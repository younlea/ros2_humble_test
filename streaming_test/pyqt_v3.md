요청에 따라 수정된 Python 코드를 제공합니다. 이번에는 아래 요구 사항을 반영했습니다.

변경/추가 사항
	1.	스트리밍 화면(FHD 전체 화면 출력)
	•	스트리밍 이미지를 QLabel에서 FHD(1920x1080)로 정확히 보여줌.
	•	QLabel 크기를 동적으로 변경하여 이미지 왜곡 없이 전체 화면 출력.
	2.	캡처 및 POI 캡처 이미지 출력
	•	Capture 버튼: 버튼 아래에 캡처 이미지를 축소하여 전체 이미지가 보이도록 표시.
	•	Capture POI 버튼: 선택된 POI 이미지를 버튼 아래에 맞춰 출력.
	3.	POI 박스가 스트리밍 화면에 계속 보임
	•	POI 박스가 설정되면 해당 박스를 스트리밍 화면에 항상 표시.

수정된 코드
```python
import sys
import cv2
import numpy as np
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QWidget
)
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import QTimer, Qt

class VideoViewer(QWidget):
    def __init__(self, rtsp_url):
        super().__init__()
        self.rtsp_url = rtsp_url
        self.init_ui()
        self.cap = cv2.VideoCapture(self.rtsp_url)

        if not self.cap.isOpened():
            print("Failed to open RTSP stream.")
            sys.exit()

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_frame)
        self.timer.start(30)

        self.current_frame = None
        self.selected_roi = None
        self.drawing_box = False
        self.start_point = None
        self.end_point = None

    def init_ui(self):
        self.setWindowTitle("RTSP Video Viewer with Capture and POI")
        
        # 메인 스트리밍 화면 (FHD 크기로 고정)
        self.image_label = QLabel(self)
        self.image_label.setFixedSize(1920, 1080)
        self.image_label.setStyleSheet("border: 1px solid black;")

        # 캡처된 이미지 출력
        self.captured_label = QLabel(self)
        self.captured_label.setFixedSize(320, 180)
        self.captured_label.setStyleSheet("border: 1px solid black;")
        self.captured_label.setAlignment(Qt.AlignCenter)

        # POI 캡처된 이미지 출력
        self.poi_label = QLabel(self)
        self.poi_label.setFixedSize(320, 180)
        self.poi_label.setStyleSheet("border: 1px solid black;")
        self.poi_label.setAlignment(Qt.AlignCenter)

        # 버튼 레이아웃
        self.capture_button = QPushButton("Capture", self)
        self.capture_button.clicked.connect(self.capture_image)

        self.poi_button = QPushButton("Capture POI", self)
        self.poi_button.clicked.connect(self.capture_poi_image)

        button_layout = QHBoxLayout()
        button_layout.addWidget(self.capture_button)
        button_layout.addWidget(self.poi_button)

        # 전체 레이아웃
        main_layout = QVBoxLayout()
        main_layout.addWidget(self.image_label)
        main_layout.addLayout(button_layout)
        main_layout.addWidget(self.captured_label)
        main_layout.addWidget(self.poi_label)

        self.setLayout(main_layout)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            self.current_frame = frame
            display_frame = frame.copy()

            # POI 박스를 표시
            if self.selected_roi:
                x, y, w, h = self.selected_roi
                cv2.rectangle(display_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

            # OpenCV의 BGR 이미지를 PyQt용 QImage로 변환
            display_frame = cv2.resize(display_frame, (1920, 1080))  # FHD 크기로 변환
            display_frame_rgb = cv2.cvtColor(display_frame, cv2.COLOR_BGR2RGB)
            height, width, channel = display_frame_rgb.shape
            bytes_per_line = 3 * width
            q_image = QImage(display_frame_rgb.data, width, height, bytes_per_line, QImage.Format_RGB888)

            # QLabel에 이미지를 표시
            self.image_label.setPixmap(QPixmap.fromImage(q_image))

    def capture_image(self):
        """현재 프레임을 캡처하여 저장 및 표시"""
        if self.current_frame is not None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"capture_{timestamp}.jpg"
            cv2.imwrite(filename, self.current_frame)
            print(f"Saved: {filename}")

            # PyQt에서 캡처된 이미지를 QLabel에 표시
            self.update_captured_image(self.captured_label, self.current_frame)

    def capture_poi_image(self):
        """POI 영역을 캡처하여 저장 및 표시"""
        if self.current_frame is not None and self.selected_roi is not None:
            x, y, w, h = self.selected_roi
            roi = self.current_frame[y:y + h, x:x + w]
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"capture_poi_{timestamp}.jpg"
            cv2.imwrite(filename, roi)
            print(f"Saved POI: {filename}")

            # PyQt에서 POI 이미지를 QLabel에 표시
            self.update_captured_image(self.poi_label, roi)

    def update_captured_image(self, label, image):
        """지정된 QLabel에 캡처된 이미지를 표시"""
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image_resized = cv2.resize(image_rgb, (320, 180))  # QLabel 크기에 맞게 조정
        height, width, channel = image_resized.shape
        bytes_per_line = 3 * width
        q_image = QImage(image_resized.data, width, height, bytes_per_line, QImage.Format_RGB888)
        label.setPixmap(QPixmap.fromImage(q_image))

    def mousePressEvent(self, event):
        """마우스 클릭 시 박스 시작점 설정"""
        if event.button() == Qt.LeftButton and self.image_label.geometry().contains(event.pos()):
            self.drawing_box = True
            self.start_point = (event.x() - self.image_label.x(), event.y() - self.image_label.y())

    def mouseMoveEvent(self, event):
        """마우스 드래그 시 박스 끝점 업데이트"""
        if self.drawing_box and self.start_point:
            self.end_point = (event.x() - self.image_label.x(), event.y() - self.image_label.y())

    def mouseReleaseEvent(self, event):
        """마우스 릴리스 시 박스 선택 완료"""
        if event.button() == Qt.LeftButton and self.drawing_box:
            self.drawing_box = False
            self.end_point = (event.x() - self.image_label.x(), event.y() - self.image_label.y())

            # 박스 영역 설정
            x1, y1 = self.start_point
            x2, y2 = self.end_point
            self.selected_roi = (
                min(x1, x2),
                min(y1, y2),
                abs(x2 - x1),
                abs(y2 - y1),
            )
            print(f"Selected ROI: {self.selected_roi}")

    def closeEvent(self, event):
        self.cap.release()

if __name__ == "__main__":
    app = QApplication(sys.argv)

    # ID와 PW를 포함한 RTSP URL
    rtsp_url = "rtsp://admin:123456@192.168.0.4"
    viewer = VideoViewer(rtsp_url)
    viewer.show()
    sys.exit(app.exec_())
```

주요 변경 사항
	1.	FHD 스트리밍 화면: self.image_label 크기를 1920x1080으로 설정하고 프레임을 cv2.resize로 크기 조정.
	2.	캡처 이미지 크기 맞춤: 각 QLabel에 맞게 캡처된 이미지를 320x180으로 축소하여 출력.
	3.	POI 박스 지속 표시: update_frame 함수에서 selected_roi가 설정되었을 때 박스를 계속 그리도록 수정.

실행 방법
	1.	필요한 패키지 설치:

pip install opencv-python-headless PyQt5


	2.	코드 실행:

python rtsp_viewer_with_poi.py

동작 확인
	1.	FHD 화면 출력: 메인 화면에 FHD로 스트리밍 영상이 표시됩니다.
	2.	캡처 버튼: 캡처된 전체 이미지가 버튼 아래 축소되어 표시됩니다.
	3.	POI 캡처 버튼: 선택된 POI 이미지가 버튼 아래 맞춰 표시됩니다.
	4.	POI 박스 지속 표시: 박스를 그리면 스트리밍 화면에 계속 표시됩니다.
