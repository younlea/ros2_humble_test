아래는 QVBoxLayout과 addWidget, addLayout을 사용해 동일한 레이아웃을 구성한 코드입니다. PyQt5 또는 PySide2 환경에서 동작합니다.

코드: init_ui 함수 수정 (PyQt5/PySide2)
```python
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QLabel, QPushButton
)
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QSize

def init_ui(self):
    # 메인 윈도우
    self.setWindowTitle("Video Streamer")
    self.setGeometry(100, 100, 960, 720)

    # 중앙 위젯과 레이아웃 설정
    central_widget = QWidget()
    self.setCentralWidget(central_widget)

    main_layout = QVBoxLayout()
    central_widget.setLayout(main_layout)

    # 1. 영상 스트리밍 영역
    self.stream_label = QLabel("Video Stream")
    self.stream_label.setFixedSize(960, 540)  # FHD 비율
    self.stream_label.setStyleSheet("background-color: black;")  # 배경색 지정
    main_layout.addWidget(self.stream_label)

    # 2. 버튼 및 캡처 이미지 영역 (수평 레이아웃)
    button_layout = QHBoxLayout()
    main_layout.addLayout(button_layout)

    # (a) 캡처 버튼과 캡처된 이미지
    capture_layout = QVBoxLayout()
    self.capture_button = QPushButton("Capture")
    self.capture_button.clicked.connect(self.capture_image)
    capture_layout.addWidget(self.capture_button)

    self.captured_label = QLabel("Captured Image")
    self.captured_label.setFixedSize(320, 240)  # 320x240 크기 설정
    self.captured_label.setStyleSheet("background-color: gray;")  # 배경색 지정
    capture_layout.addWidget(self.captured_label)

    button_layout.addLayout(capture_layout)

    # (b) POI 캡처 버튼과 캡처된 POI 이미지
    capture_poi_layout = QVBoxLayout()
    self.capture_poi_button = QPushButton("Capture POI")
    self.capture_poi_button.clicked.connect(self.capture_poi_image)
    capture_poi_layout.addWidget(self.capture_poi_button)

    self.poi_label = QLabel("POI Image")
    self.poi_label.setFixedSize(320, 240)  # 320x240 크기 설정
    self.poi_label.setStyleSheet("background-color: gray;")  # 배경색 지정
    capture_poi_layout.addWidget(self.poi_label)

    button_layout.addLayout(capture_poi_layout)
```
설명
	1.	레이아웃 구성:
	•	QVBoxLayout: 전체 레이아웃을 세로로 정렬.
	•	QHBoxLayout: 캡처 버튼과 이미지를 수평으로 정렬.
	2.	위젯 추가:
	•	addWidget: 레이블과 버튼을 추가.
	•	addLayout: 수평/수직 레이아웃을 다른 레이아웃에 추가.
	3.	UI 구조:
	•	최상단: self.stream_label (실시간 영상 스트리밍).
	•	하단 왼쪽: 캡처 버튼과 캡처된 이미지.
	•	하단 오른쪽: POI 캡처 버튼과 캡처된 POI 이미지.
	4.	위젯 크기 설정:
	•	setFixedSize: 위젯의 크기를 강제로 고정.
	•	영상 스트리밍 영역은 960x540, 이미지는 320x240으로 설정.
	5.	버튼 동작:
	•	self.capture_button.clicked.connect(self.capture_image)로 캡처 기능 연결.
	•	self.capture_poi_button.clicked.connect(self.capture_poi_image)로 POI 캡처 연결.

실행 결과

이 코드를 실행하면 다음과 같은 레이아웃이 구성됩니다:
	1.	상단: 960x540 크기의 실시간 영상 스트리밍 화면.
	2.	하단 왼쪽: 캡처 버튼과 캡처된 320x240 이미지.
	3.	하단 오른쪽: POI 캡처 버튼과 POI 캡처된 320x240 이미지.

추가 조정
	•	버튼의 크기, 배치, 스타일 변경은 QPushButton의 setStyleSheet 메서드로 가능합니다.
	•	문제가 있거나 추가 요청 사항이 있으면 말씀해주세요!
