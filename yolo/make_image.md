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
        # 캡처 타이머에 의해 호출됨: 현재 프레임에서 ROI 영역 부분을 잘라 파일로 저장
        if self.current_frame is not None and self.roi:
            x, y, w, h = self.roi
            frame_h, frame_w = self.current_frame.shape[:2]
            # ROI가 영상 경계를 벗어나지 않도록 조정
            x = max(0, min(x, frame_w - 1))
            y = max(0, min(y, frame_h - 1))
            if x + w > frame_w:
                w = frame_w - x
            if y + h > frame_h:
                h = frame_h - y
            roi_frame = self.current_frame[y:y+h, x:x+w]
            filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}.jpg")
            cv2.imwrite(filename, roi_frame)
            print("Saved:", filename)
            self.capture_count += 1
    
    def closeEvent(self, event):
        if self.cap:
            self.cap.release()
        event.accept()

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
