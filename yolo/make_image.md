아래 코드는 다음과 같은 동작을 수행합니다.

-  영상 파일을 열면 첫 번째 프레임을 표시하고, 영상은 재생되지 않으며 사용자가 그 위에서 ROI 영역을 마우스로 선택할 수 있습니다.  
-  ROI 영역은 마우스 클릭 → 드래그 → 릴리즈 과정을 통해 실시간으로 표시되며, 릴리즈 후 ROI가 확정됩니다.  
-  "Start Capture" 버튼을 누르면 영상이 재생되면서 계속해서 ROI 영역이 오버레이되고, 지정한 초당 캡처 빈도로 ROI 영역이 저장됩니다.  
-  버튼은 토글 형식으로, "Start Capture"를 누르면 영상이 재생되어 캡처가 진행되고, 도중에 "Stop Capture"를 누르면 영상 재생과 캡처가 일시 정지됩니다. 다시 "Start Capture"를 누르면 이전에 멈춘 자리부터 재개됩니다.

아래 코드를 Python 파일(예: main.py)로 저장한 후 실행하시고, 프로그램을 돌리기 전에 아래에 소개된 라이브러리를 설치하세요.

---

```python
import sys
import os
import cv2
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QFileDialog,
    QLabel, QSpinBox, QHBoxLayout, QVBoxLayout, QWidget
)
from PyQt5.QtCore import Qt, QTimer, QPoint
from PyQt5.QtGui import QPixmap, QImage

class VideoCaptureApp(QMainWindow):
    def __init__(self):
        super().__init__()
        # 기존 변수 선언 부분 위에 추가
        self.capture_buffer = []  # 캡처된 ROI 이미지를 임시 저장할 리스트

        self.setWindowTitle("Video ROI Capture")
        self.setGeometry(100, 100, 800, 600)
        
        # 변수 초기화
        self.video_path = None
        self.cap = None
        self.current_frame = None     # 현재 원본 프레임을 저장
        self.original_size = (0, 0)   # (원본 영상 가로, 세로)
        self.scale_factor = 1.0       # 원본 → 화면 변환 비율
        
        self.roi = None             # 최종 선택된 ROI (원본 좌표: (x, y, w, h))
        self.live_roi = None        # ROI 선택 중 영상 표시 좌표
        self.drawing = False        # ROI 그리기 동작 중 여부
        self.start_point = QPoint()
        self.end_point = QPoint()
        
        self.capturing = False      # 캡처 동작이 진행 중인지 여부
        self.capture_count = 1      # 저장될 이미지 파일 번호
        
        # 타이머: 영상 재생과 ROI 캡처를 위한 타이머 (일시 정지 후 재개 가능)
        self.playback_timer = QTimer()
        self.playback_timer.timeout.connect(self.update_frame)
        self.capture_timer = QTimer()
        self.capture_timer.timeout.connect(self.capture_frame)
        
        self.init_ui()
    
    def init_ui(self):
        # 메인 레이아웃 구성: 영상 표시 영역과 컨트롤 패널(버튼 및 텍스트)은 별도 처리
        main_layout = QVBoxLayout()
        
        # 영상 표시 영역 (윈도우 크기에 따라 동적으로 리사이즈, 버튼/텍스트는 고정)
        self.video_label = QLabel("Load a video to start")
        self.video_label.setAlignment(Qt.AlignCenter)
        self.video_label.setStyleSheet("background-color: black;")
        self.video_label.setSizePolicy(self.video_label.sizePolicy().Expanding,
                                       self.video_label.sizePolicy().Expanding)
        main_layout.addWidget(self.video_label)
        
        # 컨트롤 패널
        control_layout = QHBoxLayout()
        
        self.open_button = QPushButton("Open Video")
        self.open_button.clicked.connect(self.open_video)
        control_layout.addWidget(self.open_button)
        
        self.fps_label = QLabel("Captures per second:")
        control_layout.addWidget(self.fps_label)
        
        self.capture_rate_spinbox = QSpinBox()
        self.capture_rate_spinbox.setRange(1, 60)
        self.capture_rate_spinbox.setValue(5)
        control_layout.addWidget(self.capture_rate_spinbox)
        
        self.start_button = QPushButton("Start Capture")
        self.start_button.setCheckable(True)
        self.start_button.clicked.connect(self.toggle_capture)
        control_layout.addWidget(self.start_button)
        
        main_layout.addLayout(control_layout)

        self.quick_save_button = QPushButton("Quick Save")
        self.quick_save_button.clicked.connect(self.quick_save)
        control_layout.addWidget(self.quick_save_button)

        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)
        
        # 영상 표시 영역에 마우스 이벤트 연결 (ROI 선택용)
        self.video_label.mousePressEvent = self.video_label_mouse_press
        self.video_label.mouseMoveEvent = self.video_label_mouse_move
        self.video_label.mouseReleaseEvent = self.video_label_mouse_release
    
    def open_video(self):
        # 영상 파일 열기 및 첫 번째 프레임 읽기 (플레이는 하지 않음)
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Open Video File", "", "Video Files (*.avi *.mp4 *.mov)")
        if file_path:
            self.video_path = file_path
            self.cap = cv2.VideoCapture(file_path)
            self.original_size = (int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH)),
                                  int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
            # 영상 파일명 기반 캡처 폴더 생성 (예: 영상이름_captured)
            base_name = os.path.splitext(os.path.basename(file_path))[0]
            self.capture_folder = base_name + "_captured"
            if not os.path.exists(self.capture_folder):
                os.makedirs(self.capture_folder)
            self.capture_count = 1
            
            # 첫 번째 프레임 읽어와서 표시 (플레이는 시작하지 않음)
            ret, frame = self.cap.read()
            if ret:
                self.current_frame = frame.copy()
                # 영상 출발 위치를 처음으로 리셋 (다시 시작 시 정상 재생되도록)
                self.cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
                self.display_frame(frame)
            else:
                print("영상을 읽을 수 없습니다.")
    
    def display_frame(self, frame):
        # 영상 표시 영역 크기에 맞춰 프레임 크기를 조정하고, ROI 영역(실시간/최종)을 오버레이하여 출력
        resized_frame = self.resize_frame(frame)
        if self.drawing and self.live_roi:
            x, y, w, h = self.live_roi
            cv2.rectangle(resized_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        elif self.roi:
            x = int(self.roi[0] * self.scale_factor)
            y = int(self.roi[1] * self.scale_factor)
            w = int(self.roi[2] * self.scale_factor)
            h = int(self.roi[3] * self.scale_factor)
            cv2.rectangle(resized_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        
        height, width, channels = resized_frame.shape
        bytes_per_line = channels * width
        # QImage.Format_BGR888를 사용하여 원본 색상을 유지 (색상 왜곡 없이 출력)
        q_img = QImage(resized_frame.data, width, height, bytes_per_line, QImage.Format_BGR888)
        self.video_label.setPixmap(QPixmap.fromImage(q_img))
    
    def resize_frame(self, frame):
        # 영상 표시 영역 크기에 맞춰 원본 영상의 크기를 비율 유지하면서 조정
        label_size = self.video_label.size()
        scale_w = label_size.width() / self.original_size[0]
        scale_h = label_size.height() / self.original_size[1]
        self.scale_factor = min(scale_w, scale_h)
        new_width = int(self.original_size[0] * self.scale_factor)
        new_height = int(self.original_size[1] * self.scale_factor)
        resized = cv2.resize(frame, (new_width, new_height))
        return resized
    
    def update_frame(self):
        # 영상 재생 타이머에 의해 호출됨: 매 프레임마다 새로운 영상을 읽어옴
        if self.cap and self.cap.isOpened():
            ret, frame = self.cap.read()
            if ret:
                self.current_frame = frame.copy()
                self.display_frame(frame)
            else:
                # 영상이 끝나면 타이머 정지
                self.playback_timer.stop()
                self.capture_timer.stop()
                print("영상 재생이 종료되었습니다.")
    
    def video_label_mouse_press(self, event):
        if event.button() == Qt.LeftButton and self.current_frame is not None:
            self.drawing = True
            self.start_point = event.pos()
            self.end_point = event.pos()
            self.live_roi = (self.start_point.x(), self.start_point.y(), 0, 0)
            self.display_frame(self.current_frame)
    
    def video_label_mouse_move(self, event):
        if self.drawing:
            self.end_point = event.pos()
            x1 = self.start_point.x()
            y1 = self.start_point.y()
            x2 = self.end_point.x()
            y2 = self.end_point.y()
            x = min(x1, x2)
            y = min(y1, y2)
            w = abs(x2 - x1)
            h = abs(y2 - y1)
            self.live_roi = (x, y, w, h)
            self.display_frame(self.current_frame)
    
    def video_label_mouse_release(self, event):
        if event.button() == Qt.LeftButton and self.drawing:
            self.drawing = False
            self.end_point = event.pos()
            x1 = self.start_point.x()
            y1 = self.start_point.y()
            x2 = self.end_point.x()
            y2 = self.end_point.y()
            x = min(x1, x2)
            y = min(y1, y2)
            w = abs(x2 - x1)
            h = abs(y2 - y1)
            self.live_roi = (x, y, w, h)
            # display 좌표를 원본 좌표로 변환 (scale_factor 이용)
            if self.scale_factor > 0:
                self.roi = (int(x / self.scale_factor), int(y / self.scale_factor),
                            int(w / self.scale_factor), int(h / self.scale_factor))
            self.display_frame(self.current_frame)
    
    def toggle_capture(self):
        # 캡처 버튼 토글: 첫 누름시 영상 재생 + 캡처 시작, 다시 누르면 멈춤 (재개 시 이어서 진행)
        if not self.cap or self.current_frame is None:
            return
        
        if self.start_button.isChecked():
            if self.roi is None:
                print("먼저 ROI 영역을 선택하세요.")
                self.start_button.setChecked(False)
                return
            self.start_button.setText("Stop Capture")
            # 영상 재생 타이머 시작 (30ms마다 업데이트)
            self.playback_timer.start(30)
            # 캡처 타이머 시작 (사용자 입력에 따라 초당 캡처 수)
            interval = int(60000 / self.capture_rate_spinbox.value())
            self.capture_timer.start(interval)
            self.capturing = True
        else:
            # 일시 정지: 재생 및 캡처 타이머 모두 멈춤 (다음번 캡처 시 이어서 진행됨)
            self.playback_timer.stop()
            self.capture_timer.stop()
            self.start_button.setText("Start Capture")
            self.capturing = False

    def capture_frame(self):
        if self.current_frame is not None and self.roi:
            x, y, w, h = self.roi
            frame_h, frame_w = self.current_frame.shape[:2]
            x = max(0, min(x, frame_w - 1))
            y = max(0, min(y, frame_h - 1))
            if x + w > frame_w:
                w = frame_w - x
            if y + h > frame_h:
                h = frame_h - y
            roi_frame = self.current_frame[y:y+h, x:x+w]
    
            # 버퍼에 추가
            self.capture_buffer.append(roi_frame)
            
            # 버퍼에 2장 모이면 위아래로 결합해서 저장
            if len(self.capture_buffer) == 2:
                merged_image = cv2.vconcat(self.capture_buffer)
                filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}.jpg")
                cv2.imwrite(filename, merged_image)
                print("Saved:", filename)
                self.capture_count += 1
                self.capture_buffer = []  # 버퍼 초기화
    
    def closeEvent(self, event):
        if self.cap:
            self.cap.release()
        event.accept()

    def quick_save(self):
        # 영상과 ROI 영역이 선택되어야 진행
        if not self.cap or self.current_frame is None or self.roi is None:
            print("먼저 영상과 ROI 영역을 선택하세요.")
            return
    
        self.capture_buffer = []  # 기존 캡처 버퍼 초기화
        fps = self.cap.get(cv2.CAP_PROP_FPS)
        # 분당 캡처 수 설정(분당 캡처 수에 따라 건너뛰어야 할 프레임 수 계산)
        capture_interval_frames = int(round(fps * (60 / self.capture_rate_spinbox.value())))
        
        current_frame_idx = int(self.cap.get(cv2.CAP_PROP_POS_FRAMES))
        
        # quick_save에서는 GUI 업데이트 없이 빠르게 프레임 점프로 캡처 진행
        while True:
            next_frame_idx = current_frame_idx + capture_interval_frames
            self.cap.set(cv2.CAP_PROP_POS_FRAMES, next_frame_idx)
            ret, frame = self.cap.read()
            if not ret:
                break  # 영상 끝
            
            current_frame_idx = next_frame_idx
            self.current_frame = frame.copy()
            
            x, y, w, h = self.roi
            frame_h, frame_w = frame.shape[:2]
            x = max(0, min(x, frame_w - 1))
            y = max(0, min(y, frame_h - 1))
            if x + w > frame_w:
                w = frame_w - x
            if y + h > frame_h:
                h = frame_h - y
            roi_frame = frame[y:y+h, x:x+w]
            self.capture_buffer.append(roi_frame)
            if len(self.capture_buffer) == 2:
                merged_image = cv2.vconcat(self.capture_buffer)
                filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}.jpg")
                cv2.imwrite(filename, merged_image)
                print("Quick Saved:", filename)
                self.capture_count += 1
                self.capture_buffer = []
        
        if len(self.capture_buffer) == 1:
            merged_image = self.capture_buffer[0]
            filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}_single.jpg")
            cv2.imwrite(filename, merged_image)
            print("Quick Saved (single):", filename)
            self.capture_count += 1
            self.capture_buffer = []




if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = VideoCaptureApp()
    window.show()
    sys.exit(app.exec_())
```

---

### 실행 전 설치할 라이브러리

Mac에서 conda 환경 내에 아래 명령어를 통해 필요한 라이브러리를 설치하세요.

```bash
conda install -c conda-forge pyqt
pip install opencv-python
```

위 코드를 실행하면 먼저 "Open Video" 버튼으로 영상 파일을 선택한 후, 화면에 첫 번째 프레임이 표시됩니다. 그 후 영상 위에서 마우스로 ROI 영역을 선택하면 해당 영역이 실시간으로 표시되며, "Start Capture" 버튼을 누르면 영상이 재생되면서 ROI 영역은 계속 표시되고, 지정한 초당 캡처 수에 따라 ROI 영역 이미지가 순차적으로 저장됩니다. "Stop Capture" 버튼을 누르면 일시 정지되며, 이후 다시 "Start Capture"를 누르면 이전 위치부터 캡처가 재개됩니다.

---
Perplexity로부터의 답변: pplx.ai/share


---
좋은 질문입니다! 업스케일링을 하면 이미지의 크기는 커지지만, **실제 해상도가 높아지는 것은 아닙니다.** 업스케일링은 기존 픽셀 데이터를 보간(interpolation)하여 더 큰 크기로 확대하는 방식이므로, 원본 데이터에 없는 디테일을 생성할 수는 없습니다. 결과적으로 해상도는 높아지지 않고, 단지 이미지의 크기만 커질 뿐입니다.

### 문제의 원인: 초기 파일 열 때 이미지 크기 설정
말씀하신 대로, **초기에 파일을 열 때 이미지 크기를 잘못 처리했거나 압축된 상태로 저장했을 가능성**도 있습니다. OpenCV에서 이미지를 처리할 때, 기본적으로 원본 해상도를 유지하지만, 특정 상황에서는 이미지가 축소되거나 압축 설정이 잘못될 수 있습니다.

따라서 이 문제를 해결하려면 다음 두 가지를 확인해야 합니다:

---

### 1. **원본 프레임 크기 확인**
`cv2.VideoCapture`를 사용하여 영상을 열었을 때, 실제로 읽어온 프레임이 원본 해상도(1920x1080)인지 확인해야 합니다. 이를 위해 다음 코드를 추가하여 디버깅합니다:

```python
def open_video(self):
    file_path, _ = QFileDialog.getOpenFileName(
        self, "Open Video File", "", "Video Files (*.avi *.mp4 *.mov)")
    if file_path:
        self.video_path = file_path
        self.cap = cv2.VideoCapture(file_path)
        self.original_size = (
            int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH)),
            int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        )
        print(f"Original video resolution: {self.original_size}")  # 원본 해상도 출력
```

위 코드를 통해 `self.original_size` 값을 확인하세요. 만약 `self.original_size`가 1920x1080이 아니라 작게 나오는 경우, 영상 파일 자체가 축소된 상태로 저장되어 있거나 OpenCV에서 잘못 읽어왔을 가능성이 있습니다.

---

### 2. **저장 시 압축 품질 확인**
OpenCV의 `cv2.imwrite` 함수는 기본적으로 JPEG 이미지를 저장할 때 품질(압축률)을 설정할 수 있습니다. 기본 설정은 품질이 낮게 설정되어 있을 수 있으므로, 저장 시 품질 옵션을 명시적으로 설정해야 합니다.

아래와 같이 `cv2.imwrite`에 추가 옵션을 전달하여 JPEG 품질을 높일 수 있습니다:

```python
filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}.jpg")
cv2.imwrite(filename, merged_image, [cv2.IMWRITE_JPEG_QUALITY, 100])  # 품질 100으로 설정
```

여기서 `IMWRITE_JPEG_QUALITY` 값은 0~100 사이의 정수로 설정하며, 값이 클수록 품질이 높고 파일 크기도 커집니다. 기본값은 약 95 정도이며, 필요에 따라 100으로 설정하면 가장 높은 품질로 저장됩니다.

---

### 최종 점검 및 수정 방향
1. **원본 해상도 확인**: 위에서 설명한 대로 `self.original_size`를 출력하여 OpenCV가 올바른 해상도로 영상을 읽어오는지 확인합니다.
    - 만약 해상도가 작게 나왔다면 OpenCV 대신 다른 라이브러리(예: FFmpeg)를 사용하거나 영상 파일 자체를 점검해야 합니다.
2. **저장 시 압축률 조정**: `cv2.imwrite`에 `IMWRITE_JPEG_QUALITY` 옵션을 추가하여 저장 품질을 높입니다.
3. **업스케일링 여부 판단**: 원본 해상도가 제대로 유지되고 있다면 업스케일링은 필요하지 않습니다. 그러나 원본 영상 자체가 작은 경우에는 업스케일링이 필요할 수 있습니다.

---

### 결론
- 만약 원본 영상이 제대로 읽히고 있다면, 문제는 **저장 시 압축률** 때문일 가능성이 높습니다. 이 경우 `cv2.imwrite`의 `IMWRITE_JPEG_QUALITY` 옵션으로 해결할 수 있습니다.
- 반대로 OpenCV가 영상을 작게 읽어온다면 이는 OpenCV의 영상 처리 방식이나 영상 파일 자체 문제일 수 있으므로 원본 해상도를 확인하고 필요하면 다른 도구를 사용해 영상을 처리해야 합니다.

위 내용을 바탕으로 문제를 점검하고 수정하면 학습용 데이터로 적합한 고품질 이미지를 얻을 수 있을 것입니다! 😊

출처

