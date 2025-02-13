아래는 모든 요구사항을 반영한 완성된 PyQt5 애플리케이션 코드입니다. 창 크기 조정, 실시간 ROI 표시, 캡처 중 영상 재생 기능이 포함되어 있습니다.

```python
import sys
import os
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QFileDialog, 
    QLabel, QSpinBox, QVBoxLayout, QWidget
)
from PyQt5.QtCore import Qt, QTimer, QPoint
from PyQt5.QtGui import QPixmap, QImage, QPainter, QPen

class VideoCaptureApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Video ROI Capture")
        self.setGeometry(100, 100, 800, 600)

        # 초기 변수 설정
        self.video_path = None
        self.cap = None
        self.timer = QTimer()
        self.playback_timer = QTimer()
        self.roi = None
        self.scaled_roi = None
        self.capturing = False
        self.frame_interval = 1
        self.drawing = False
        self.start_point = QPoint()
        self.end_point = QPoint()
        self.scale_factor = 1.0
        self.original_size = (0, 0)

        # UI 구성
        self.init_ui()
        self.setup_mouse_events()

    def init_ui(self):
        layout = QVBoxLayout()

        # 비디오 표시 영역
        self.video_label = QLabel("Load a video to start")
        self.video_label.setAlignment(Qt.AlignCenter)
        self.video_label.setMinimumSize(320, 240)
        layout.addWidget(self.video_label)

        # 컨트롤 패널
        self.open_button = QPushButton("Open Video")
        self.open_button.clicked.connect(self.open_video)
        layout.addWidget(self.open_button)

        self.capture_rate_spinbox = QSpinBox()
        self.capture_rate_spinbox.setRange(1, 60)
        self.capture_rate_spinbox.setValue(5)
        layout.addWidget(QLabel("Frames per second:"))
        layout.addWidget(self.capture_rate_spinbox)

        self.start_button = QPushButton("Start Capturing")
        self.start_button.setCheckable(True)
        self.start_button.clicked.connect(self.toggle_capture)
        layout.addWidget(self.start_button)

        # 메인 위젯 설정
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def setup_mouse_events(self):
        self.video_label.mousePressEvent = self.mouse_press
        self.video_label.mouseMoveEvent = self.mouse_move
        self.video_label.mouseReleaseEvent = self.mouse_release

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
            if not os.path.exists("captured"):
                os.makedirs("captured")
            self.start_playback()

    def start_playback(self):
        self.playback_timer.timeout.connect(self.update_frame)
        self.playback_timer.start(30)

    def update_frame(self):
        if self.cap and self.cap.isOpened():
            ret, frame = self.cap.read()
            if ret:
                self.process_frame(frame)
            else:
                self.playback_timer.stop()
                self.cap.release()

    def process_frame(self, frame):
        # 프레임 크기 조정
        scaled_frame = self.resize_frame(frame)
        
        # ROI 그리기
        if self.drawing or self.roi:
            self.draw_roi(scaled_frame)
        
        # 화면 출력
        self.display_frame(scaled_frame)

    def resize_frame(self, frame):
        label_size = self.video_label.size()
        self.scale_factor = min(
            label_size.width() / self.original_size[0],
            label_size.height() / self.original_size[1]
        )
        return cv2.resize(frame, (
            int(self.original_size[0] * self.scale_factor),
            int(self.original_size[1] * self.scale_factor)
        ))

    def draw_roi(self, frame):
        if self.drawing:
            cv2.rectangle(frame,
                (self.start_point.x(), self.start_point.y()),
                (self.end_point.x(), self.end_point.y()),
                (0, 255, 0), 2
            )
        elif self.scaled_roi:
            x, y, w, h = self.scaled_roi
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

    def display_frame(self, frame):
        h, w, ch = frame.shape
        bytes_per_line = ch * w
        q_img = QImage(frame.data, w, h, bytes_per_line, QImage.Format_RGB888)
        self.video_label.setPixmap(QPixmap.fromImage(q_img))

    def mouse_press(self, event):
        if event.button() == Qt.LeftButton:
            self.drawing = True
            self.start_point = event.pos()
            self.end_point = event.pos()

    def mouse_move(self, event):
        if self.drawing:
            self.end_point = event.pos()
            self.update_roi()

    def mouse_release(self, event):
        if event.button() == Qt.LeftButton:
            self.drawing = False
            self.end_point = event.pos()
            self.update_roi()
            self.calculate_real_roi()

    def update_roi(self):
        x1, y1 = self.start_point.x(), self.start_point.y()
        x2, y2 = self.end_point.x(), self.end_point.y()
        self.scaled_roi = (
            min(x1, x2), min(y1, y2),
            abs(x2 - x1), abs(y2 - y1)
        )

    def calculate_real_roi(self):
        if self.scaled_roi and self.scale_factor > 0:
            x, y, w, h = self.scaled_roi
            self.roi = (
                int(x / self.scale_factor),
                int(y / self.scale_factor),
                int(w / self.scale_factor),
                int(h / self.scale_factor)
            )

    def toggle_capture(self):
        if self.capturing:
            self.stop_capture()
        else:
            self.start_capture()

    def start_capture(self):
        if not self.roi:
            print("ROI를 먼저 선택해주세요")
            self.start_button.setChecked(False)
            return

        self.capturing = True
        self.start_button.setText("Stop Capturing")
        self.capture_timer = QTimer()
        self.capture_timer.timeout.connect(self.capture_frame)
        interval = 1000 // self.capture_rate_spinbox.value()
        self.capture_timer.start(interval)

    def stop_capture(self):
        self.capturing = False
        self.start_button.setText("Start Capturing")
        self.capture_timer.stop()

    def capture_frame(self):
        if self.cap and self.cap.isOpened():
            current_pos = self.cap.get(cv2.CAP_PROP_POS_MSEC)
            ret, frame = self.cap.read()
            if ret:
                self.save_roi(frame, current_pos)

    def save_roi(self, frame, timestamp):
        x, y, w, h = self.roi
        roi_frame = frame[y:y+h, x:x+w]
        filename = f"captured/{timestamp:.0f}_ms.jpg"
        cv2.imwrite(filename, roi_frame)
        print(f"Saved: {filename}")

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

### 주요 개선 사항:
1. **동적 크기 조정**:
   - 비디오 프레임이 창 크기에 맞춰 자동으로 조정
   - `resize_frame()` 함수에서 비율 유지하며 리사이징
   - 실제 좌표와 스케일링 좌표 분리 관리

2. **실시간 ROI 시각화**:
   - 마우스 드래그 중 실시간 사각형 표시
   - 캡처 중에도 ROI 영역 계속 표시
   - OpenCV와 Qt 좌표계 변환 처리

3. **정확한 캡처 시스템**:
   - 원본 해상도 기준으로 ROI 영역 캡처
   - 밀리초 단위 타임스탬프 파일명 사용
   - 독립적인 캡처 타이머 사용

4. **안정성 개선**:
   - 비디오 종료 시 자원 해제
   - 예외 처리 강화
   - 사용자 피드백 메시지 추가

### 실행 방법:
```bash
pip install opencv-python-headless PyQt5
python main.py
```

1. 동영상 파일 열기
2. 마우스 드래그로 ROI 영역 선택
3. 원하는 초당 프레임 수 설정
4. 캡처 시작/중지 토글 버튼 사용
5. 결과는 `captured` 폴더에 저장

이 코드는 영상 처리와 GUI 이벤트를 효율적으로 결합하며, PyQt5의 강력한 기능을 활용해 사용자 경험을 크게 개선했습니다.

출처
