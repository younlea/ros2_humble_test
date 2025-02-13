PyQt5와 OpenCV를 활용한 영상 캡처 프로그램을 완성형 코드로 구현해드리겠습니다. 아래 코드는 사용자의 모든 요구사항을 충족하며 주석을 추가해 이해를 돕습니다.

```python
import sys
import os
import cv2
import numpy as np
from PyQt5.QtCore import Qt, QTimer, QPoint, QRect
from PyQt5.QtGui import QImage, QPixmap, QPainter, QPen, QMouseEvent
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QLabel, 
                            QPushButton, QFileDialog, QLineEdit,
                            QVBoxLayout, QHBoxLayout, QMessageBox)

class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt Video Capturer")
        self.setGeometry(100, 100, 800, 600)

        # 영상 처리 관련 변수
        self.cap = None
        self.frame = None
        self.roi = None
        self.drawing = False
        self.start_point = QPoint()
        self.end_point = QPoint()
        self.capture_active = False
        self.save_count = 1
        self.capture_interval = 1  # 초 단위

        # UI 초기화
        self.init_ui()
        self.init_timers()

    def init_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout()

        # 영상 표시 영역
        self.video_label = QLabel()
        self.video_label.setAlignment(Qt.AlignCenter)
        self.video_label.setMouseTracking(True)
        self.video_label.mousePressEvent = self.mouse_press
        self.video_label.mouseMoveEvent = self.mouse_move
        self.video_label.mouseReleaseEvent = self.mouse_release

        # 컨트롤 패널
        control_layout = QHBoxLayout()
        
        self.btn_open = QPushButton("Open Video")
        self.btn_open.clicked.connect(self.open_file)
        
        self.interval_input = QLineEdit("1")
        self.interval_input.setPlaceholderText("Capture Interval (seconds)")
        self.interval_input.setFixedWidth(150)
        
        self.btn_capture = QPushButton("Start Capture")
        self.btn_capture.clicked.connect(self.toggle_capture)
        self.btn_capture.setEnabled(False)

        control_layout.addWidget(self.btn_open)
        control_layout.addWidget(self.interval_input)
        control_layout.addWidget(self.btn_capture)
        
        layout.addWidget(self.video_label)
        layout.addLayout(control_layout)
        central_widget.setLayout(layout)

    def init_timers(self):
        # 영상 재생 타이머
        self.video_timer = QTimer(self)
        self.video_timer.timeout.connect(self.update_frame)
        
        # 캡처 타이머
        self.capture_timer = QTimer(self)
        self.capture_timer.timeout.connect(self.capture_roi)

    def open_file(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(
            self, "Open Video File", "", 
            "Video Files (*.avi *.mp4 *.mkv);;All Files (*)", 
            options=options
        )
        
        if file_name:
            self.cap = cv2.VideoCapture(file_name)
            if not self.cap.isOpened():
                QMessageBox.critical(self, "Error", "Cannot open video file")
                return
            
            self.fps = self.cap.get(cv2.CAP_PROP_FPS)
            self.video_timer.start(1000 // self.fps)
            self.base_name = os.path.splitext(os.path.basename(file_name))[0]
            self.save_dir = f"{self.base_name}_captured"
            os.makedirs(self.save_dir, exist_ok=True)
            self.btn_capture.setEnabled(True)

    def update_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.video_timer.stop()
            return
            
        self.frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        height, width, channel = self.frame.shape
        bytes_per_line = 3 * width
        
        # ROI 그리기
        img = QImage(self.frame.data, width, height, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(img)
        
        if self.drawing or not self.start_point.isNull():
            painter = QPainter(pixmap)
            painter.setPen(QPen(Qt.red, 2, Qt.SolidLine))
            painter.drawRect(QRect(self.start_point, self.end_point))
            painter.end()
        
        self.video_label.setPixmap(pixmap.scaled(
            self.video_label.size(), Qt.KeepAspectRatio, Qt.SmoothTransformation
        ))

    def mouse_press(self, event: QMouseEvent):
        if self.cap is None: return
        self.drawing = True
        self.start_point = event.pos()
        self.end_point = event.pos()

    def mouse_move(self, event: QMouseEvent):
        if self.drawing and self.cap is not None:
            self.end_point = event.pos()
            self.update_frame()

    def mouse_release(self, event: QMouseEvent):
        if self.cap is None: return
        self.drawing = False
        self.calculate_roi()

    def calculate_roi(self):
        label_width = self.video_label.width()
        label_height = self.video_label.height()
        
        # 실제 프레임 크기 계산
        frame_width = self.cap.get(cv2.CAP_PROP_FRAME_WIDTH)
        frame_height = self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
        
        # 좌표 변환
        x_ratio = frame_width / label_width
        y_ratio = frame_height / label_height
        
        x1 = int(min(self.start_point.x(), self.end_point.x()) * x_ratio)
        y1 = int(min(self.start_point.y(), self.end_point.y()) * y_ratio)
        x2 = int(max(self.start_point.x(), self.end_point.x()) * x_ratio)
        y2 = int(max(self.start_point.y(), self.end_point.y()) * y_ratio)
        
        self.roi = (x1, y1, x2-x1, y2-y1)

    def toggle_capture(self):
        self.capture_active = not self.capture_active
        if self.capture_active:
            try:
                interval = float(self.interval_input.text())
                if interval <= 0: raise ValueError
                self.capture_interval = interval
            except ValueError:
                QMessageBox.warning(self, "Invalid Input", "Please enter a valid number (>0)")
                self.capture_active = False
                return
            
            if self.roi is None:
                QMessageBox.warning(self, "Warning", "Please select ROI first")
                self.capture_active = False
                return
            
            self.btn_capture.setText("Stop Capture")
            self.capture_timer.start(int(self.capture_interval * 1000))
        else:
            self.btn_capture.setText("Start Capture")
            self.capture_timer.stop()

    def capture_roi(self):
        if self.frame is None or self.roi is None: return
        
        x, y, w, h = self.roi
        roi_frame = self.frame[y:y+h, x:x+w]
        
        if roi_frame.size == 0:
            QMessageBox.warning(self, "Error", "Invalid ROI selection")
            return
        
        save_path = os.path.join(self.save_dir, f"{self.base_name}_{self.save_count:04d}.jpg")
        cv2.imwrite(save_path, cv2.cvtColor(roi_frame, cv2.COLOR_RGB2BGR))
        self.save_count += 1

    def closeEvent(self, event):
        if self.cap is not None:
            self.cap.release()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```

이 코드의 주요 기능과 사용 방법은 다음과 같습니다:

1. **파일 열기**
   - "Open Video" 버튼으로 AVI 파일 포함 다양한 비디오 형식 지원
   - 동영상 로드 시 자동으로 '_captured' 폴더 생성

2. **ROI 선택**
   - 마우스 드래그로 영역 선택 (빨간 사각형 표시)
   - 실시간 드래그 프리뷰 지원
   - 좌표 변환을 통해 다양한 해상도 영상 지원

3. **캡처 제어**
   - 캡처 간격 입력 (초 단위)
   - Start/Stop 토글 버튼
   - 지정된 간격으로 ROI 영역 자동 저장
   - 연속 번호 파일명 생성 (ex: video_0001.jpg)

4. **에러 처리**
   - 잘못된 ROI 선택 시 경고
   - 숫자 입력 검증
   - 파일 열기 실패 처리

5. **추가 기능**
   - 창 크기 조절 시 비율 유지
   - 부드러운 영상 렌더링
   - 프로그램 종료 시 자원 정리

사용 방법:
1. 프로그램 실행 후 "Open Video" 버튼으로 동영상 선택
2. 영상이 표시되면 마우스 드래그로 관심 영역(ROI) 선택
3. 캡처 간격(초) 입력
4. "Start Capture" 버튼 클릭하여 캡처 시작
5. "Stop Capture" 버튼으로 캡처 중지

이 코드는 PyQt5의 이벤트 처리와 OpenCV의 영상 처리 기능을 결합하여 실시간 영상 처리 애플리케이션을 구현한 사례입니다. 타이머를 사용한 이중 업데이트 시스템(영상 재생/캡처)이 핵심 동작 원리입니다.

출처
