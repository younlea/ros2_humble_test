아래는 요청하신 두 가지 기능을 추가한 수정된 Python 코드입니다.

기능 설명
	1.	캡처 버튼:
	•	오른쪽 상단에 캡처 버튼 추가.
	•	버튼을 누르면 현재 시간을 파일 이름으로 저장한 캡처 이미지를 오른쪽 하단에 표시.
	2.	POI 캡처 버튼:
	•	화면에서 박스를 그릴 수 있는 기능 추가.
	•	박스를 그린 후 “POI 캡처” 버튼을 누르면 선택된 영역만 캡처하여 저장하고 하단에 표시.

수정된 Python 코드

import sys
import cv2
import numpy as np
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QWidget, QFileDialog
)
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import QTimer, Qt, QRect

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
        self.timer.start(30)  # 30ms마다 새 프레임 업데이트

        self.current_frame = None
        self.selected_roi = None
        self.drawing_box = False
        self.start_point = None
        self.end_point = None

    def init_ui(self):
        self.setWindowTitle("RTSP Video Viewer with Capture and POI")
        
        # 영상 출력 영역
        self.image_label = QLabel(self)
        self.image_label.setFixedSize(640, 480)

        # 캡처된 이미지 출력 영역
        self.captured_label = QLabel(self)
        self.captured_label.setFixedSize(320, 240)
        self.captured_label.setStyleSheet("border: 1px solid black;")
        self.captured_label.setAlignment(Qt.AlignCenter)

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

        self.setLayout(main_layout)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            self.current_frame = frame
            display_frame = frame.copy()

            # 박스를 그리고 있을 때 미리 보기 표시
            if self.drawing_box and self.start_point and self.end_point:
                cv2.rectangle(display_frame, self.start_point, self.end_point, (0, 255, 0), 2)

            # OpenCV의 BGR 이미지를 PyQt용 QImage로 변환
            height, width, channel = display_frame.shape
            bytes_per_line = 3 * width
            q_image = QImage(display_frame.data, width, height, bytes_per_line, QImage.Format_RGB888)

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
            self.update_captured_image(self.current_frame)

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
            self.update_captured_image(roi)

    def update_captured_image(self, image):
        """캡처된 이미지를 QLabel에 업데이트"""
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        height, width, channel = image_rgb.shape
        bytes_per_line = 3 * width
        q_image = QImage(image_rgb.data, width, height, bytes_per_line, QImage.Format_RGB888)
        self.captured_label.setPixmap(QPixmap.fromImage(q_image))

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
    rtsp_url = "rtsp://admin:123456@192.168.0.4"  # ID: admin, PW: 123456
    viewer = VideoViewer(rtsp_url)
    viewer.show()
    sys.exit(app.exec_())

주요 추가 및 변경 사항
	1.	캡처 버튼:
	•	capture_image 메서드에서 현재 프레임을 캡처하고 저장하며, 저장한 이미지를 QLabel에 표시.
	2.	POI 캡처 버튼:
	•	capture_poi_image 메서드에서 선택한 박스 영역만 캡처하고 저장하며, QLabel에 표시.
	3.	박스 그리기:
	•	mousePressEvent, mouseMoveEvent, mouseReleaseEvent를 활용해 마우스 입력으로 박스 영역을 설정.
	•	self.selected_roi에 선택한 박스 영역의 좌표 및 크기 저장.
	4.	이미지 표시 업데이트:
	•	update_captured_image 메서드를 통해 캡처된 이미지를 QLabel에 업데이트.

실행 방법
	1.	필요 라이브러리 설치:

pip install opencv-python-headless PyQt5


	2.	코드 실행:

python rtsp_viewer_with_poi.py



사용법
	1.	Capture 버튼: 현재 화면을 저장하고 QLabel에 표시.
	2.	Capture POI 버튼: 박스 영역을 설정한 뒤 해당 영역만 캡처하여 저장 및 QLabel에 표시.
