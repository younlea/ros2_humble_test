아래는 전체 기능을 구현한 완성된 코드입니다. 이 코드는 영상 파일을 열어 창 크기에 맞춰 영상 표시 영역만 동적으로 크기가 변경되고, 마우스로 ROI를 선택하면 실시간으로 선택 영역이 화면에 표시됩니다. 캡처 버튼을 토글하면 영상은 계속 재생되면서 지정한 초당 캡처 빈도로 선택된 ROI 영역이 순차적인 번호로 저장됩니다.

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
        self.current_frame = None      # 원본 프레임을 저장
        self.original_size = (0, 0)    # (원본 영상 가로, 세로)
        self.scale_factor = 1.0        # 원본 → 화면 변환 비율
        
        self.roi = None              # 최종 선택된 ROI (원본 좌표; (x,y,w,h))
        self.live_roi = None         # 마우스 드래그시 표시할 ROI (영상 표시 좌표)
        self.drawing = False         # ROI 선택 중 여부
        self.start_point = QPoint()
        self.end_point = QPoint()
        self.capture_count = 1       # 캡처된 이미지 파일명 번호
        
        # 타이머 설정 (영상 재생 및 ROI 캡처용)
        self.playback_timer = QTimer()
        self.playback_timer.timeout.connect(self.update_frame)
        self.capture_timer = QTimer()
        self.capture_timer.timeout.connect(self.capture_frame)
        
        # UI 생성
        self.init_ui()
    
    def init_ui(self):
        main_layout = QVBoxLayout()
        
        # 영상 표시 영역 (창 크기 변경에 따라 동적 리사이즈)
        self.video_label = QLabel("Load a video to start")
        self.video_label.setAlignment(Qt.AlignCenter)
        self.video_label.setStyleSheet("background-color: black;")
        self.video_label.setSizePolicy(self.video_label.sizePolicy().Expanding,
                                       self.video_label.sizePolicy().Expanding)
        main_layout.addWidget(self.video_label)
        
        # 컨트롤 패널 (버튼과 텍스트는 고정)
        control_layout = QHBoxLayout()
        
        self.open_button = QPushButton("Open Video")
        self.open_button.clicked.connect(self.open_video)
        control_layout.addWidget(self.open_button)
        
        self.fps_label = QLabel("Frames per second:")
        control_layout.addWidget(self.fps_label)
        
        self.capture_rate_spinbox = QSpinBox()
        self.capture_rate_spinbox.setRange(1, 60)
        self.capture_rate_spinbox.setValue(5)
        control_layout.addWidget(self.capture_rate_spinbox)
        
        self.start_button = QPushButton("Start Capturing")
        self.start_button.setCheckable(True)
        self.start_button.clicked.connect(self.toggle_capture)
        control_layout.addWidget(self.start_button)
        
        main_layout.addLayout(control_layout)
        
        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)
        
        # 영상 영역에 마우스 이벤트 연결 (ROI 선택용)
        self.video_label.mousePressEvent = self.video_label_mouse_press
        self.video_label.mouseMoveEvent = self.video_label_mouse_move
        self.video_label.mouseReleaseEvent = self.video_label_mouse_release
    
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
            # 영상 이름 기반 캡처 폴더 생성 (예: video1_captured)
            base_name = os.path.splitext(os.path.basename(file_path))[0]
            self.capture_folder = base_name + "_captured"
            if not os.path.exists(self.capture_folder):
                os.makedirs(self.capture_folder)
            self.capture_count = 1
            self.playback_timer.start(30)  # 약 30 FPS 갱신
    
    def update_frame(self):
        if self.cap and self.cap.isOpened():
            ret, frame = self.cap.read()
            if ret:
                self.current_frame = frame.copy()
                scaled_frame = self.resize_frame(frame)
                
                # ROI 그리기: 드래그 중이면 live_roi, 완료 후에는 self.roi
                if self.drawing and self.live_roi:
                    x, y, w, h = self.live_roi
                    cv2.rectangle(scaled_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                elif self.roi:
                    x = int(self.roi[0] * self.scale_factor)
                    y = int(self.roi[1] * self.scale_factor)
                    w = int(self.roi[2] * self.scale_factor)
                    h = int(self.roi[3] * self.scale_factor)
                    cv2.rectangle(scaled_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                
                # 색상을 원본 그대로 표현 (BGR 형식)
                height, width, channels = scaled_frame.shape
                bytes_per_line = channels * width
                q_img = QImage(scaled_frame.data, width, height, bytes_per_line, QImage.Format_BGR888)
                self.video_label.setPixmap(QPixmap.fromImage(q_img))
            else:
                # 영상이 끝나면 캡처 자원 해제 및 타이머 중지
                self.cap.release()
                self.playback_timer.stop()
    
    def resize_frame(self, frame):
        label_size = self.video_label.size()
        scale_w = label_size.width() / self.original_size[0]
        scale_h = label_size.height() / self.original_size[1]
        self.scale_factor = min(scale_w, scale_h)
        new_width = int(self.original_size[0] * self.scale_factor)
        new_height = int(self.original_size[1] * self.scale_factor)
        resized = cv2.resize(frame, (new_width, new_height))
        return resized
    
    # 마우스를 이용한 ROI 선택 (영상 표시 영역에서 좌표는 display coordinate)
    def video_label_mouse_press(self, event):
        if event.button() == Qt.LeftButton:
            self.drawing = True
            self.start_point = event.pos()
            self.end_point = event.pos()
            self.live_roi = (self.start_point.x(), self.start_point.y(), 0, 0)
    
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
    
    def video_label_mouse_release(self, event):
        if event.button() == Qt.LeftButton:
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
            # 변환: display 좌표 → 원본 좌표
            if self.scale_factor > 0:
                self.roi = (int(x / self.scale_factor), int(y / self.scale_factor),
                            int(w / self.scale_factor), int(h / self.scale_factor))
    
    def toggle_capture(self):
        if not self.cap or not self.cap.isOpened():
            return
        
        if self.start_button.isChecked():
            if not self.roi:
                print("먼저 ROI 영역을 선택하세요.")
                self.start_button.setChecked(False)
                return
            # 캡처 시작: 지정된 초당 캡처 빈도에 따라 타이머 동작
            interval = int(1000 / self.capture_rate_spinbox.value())
            self.capture_timer.start(interval)
            self.start_button.setText("Stop Capturing")
        else:
            self.capture_timer.stop()
            self.start_button.setText("Start Capturing")
    
    def capture_frame(self):
        if self.current_frame is not None and self.roi:
            x, y, w, h = self.roi
            # ROI가 원본 영상 경계를 벗어나지 않도록 조정
            frame_h, frame_w = self.current_frame.shape[:2]
            x = max(0, min(x, frame_w - 1))
            y = max(0, min(y, frame_h - 1))
            if x + w > frame_w:
                w = frame_w - x
            if y + h > frame_h:
                h = frame_h - y
            roi_frame = self.current_frame[y:y+h, x:x+w]
            filename = os.path.join(self.capture_folder, f"{self.capture_count:04d}.jpg")
            cv2.imwrite(filename, roi_frame)
            print(f"Saved: {filename}")
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

**프로그램 실행을 위한 라이브러리 설치 방법**  
아래 명령어를 터미널에서 실행하여 필요한 라이브러리를 설치합니다.

```bash
pip install opencv-python PyQt5
```

이 코드를 실행한 후, "Open Video" 버튼을 눌러 원하는 영상을 선택하고, 영상 위에서 마우스로 ROI 영역을 선택한 다음, 초당 캡처 프레임 수를 설정하고 "Start Capturing" 버튼을 토글하여 캡처를 시작하거나 중지할 수 있습니다. 저장된 캡처는 영상 이름에 “_captured”가 붙은 폴더에 순차적으로 저장됩니다.

위 코드와 설치 방법을 통해 프로그램을 바로 실행해 볼 수 있습니다.

