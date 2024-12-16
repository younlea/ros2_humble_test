Ubuntu에서 GUI를 만들어 RTSP 스트리밍 영상을 표시하고 해당 영상을 AI 처리로 연결하는 프로그램을 작성하려면, OpenCV와 PyQt (또는 C++에서 Qt)를 사용하는 것이 일반적입니다. 아래는 Python을 사용한 예제로, OpenCV로 RTSP 스트리밍을 읽고 PyQt5로 GUI에 영상을 표시하는 코드입니다. AI 처리를 위한 간단한 프레임 분석 기능도 포함되어 있습니다.

Python 코드 예제

import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import QTimer

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

    def init_ui(self):
        self.setWindowTitle("RTSP Video Viewer with AI Processing")
        self.image_label = QLabel(self)
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.image_label)
        self.setLayout(self.layout)

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            # 간단한 AI 처리 (프레임을 흑백으로 변환하는 예제)
            processed_frame = self.simple_ai_processing(frame)

            # OpenCV의 BGR 이미지를 PyQt용 QImage로 변환
            height, width, channel = processed_frame.shape
            bytes_per_line = 3 * width
            q_image = QImage(processed_frame.data, width, height, bytes_per_line, QImage.Format_RGB888)

            # QLabel에 이미지를 표시
            self.image_label.setPixmap(QPixmap.fromImage(q_image))

    def simple_ai_processing(self, frame):
        # AI 처리를 위한 샘플 함수 (여기서는 흑백 변환)
        gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        processed_frame = cv2.cvtColor(gray_frame, cv2.COLOR_GRAY2RGB)  # QLabel 표시를 위해 다시 RGB 변환
        return processed_frame

    def closeEvent(self, event):
        self.cap.release()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    viewer = VideoViewer("rtsp://192.168.0.4")  # RTSP 주소 입력
    viewer.show()
    sys.exit(app.exec_())

주요 설명
	1.	RTSP 스트림 읽기:
	•	cv2.VideoCapture로 RTSP 주소에서 영상 스트리밍 데이터를 읽습니다.
	•	연결이 실패하면 프로그램을 종료합니다.
	2.	PyQt5 GUI:
	•	QLabel을 사용해 이미지를 표시하고 QTimer로 30ms마다 프레임을 업데이트합니다.
	3.	AI 처리:
	•	simple_ai_processing 함수에서 AI 처리를 구현합니다. 현재는 흑백 변환을 예제로 제공했습니다. 이 부분에 AI 모델을 로드하고 처리 로직을 추가할 수 있습니다.
	4.	종료 처리:
	•	GUI를 닫으면 closeEvent가 호출되어 cv2.VideoCapture 리소스를 정리합니다.

실행 방법
	1.	Python 라이브러리 설치:

pip install opencv-python-headless PyQt5


	2.	프로그램 실행:

python rtsp_viewer.py

확장 가능성
	1.	AI 모델 통합:
	•	PyTorch 또는 TensorFlow 모델을 로드하여 simple_ai_processing 함수에 적용하면 AI 처리를 쉽게 통합할 수 있습니다.
	2.	버튼 추가:
	•	PyQt5에서 버튼, 슬라이더 등을 추가하여 실시간으로 AI 모델 변경이나 설정을 조정할 수 있습니다.
	3.	저장 기능:
	•	프레임 데이터를 저장하거나, AI 결과를 별도의 로그로 출력하도록 구현할 수 있습니다.
