아래 코드는 PyQt5를 이용해 스트리밍 주소와 메뉴 옵션을 선택하는 GUI 창을 먼저 띄운 후, 사용자가 선택한 RTSP 주소로 기존에 구현한 스트리밍 GUI(VideoViewer)를 호출하는 예제입니다. 만약 "ModuleNotFoundError: No module named 'PyQt5'" 에러가 발생한다면 PyQt5가 설치되어 있지 않은 것이므로 pip를 이용해 설치해 주세요.

## 예제 코드

```python
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QComboBox, QLabel
import sys

# 기존에 제작한 스트리밍 GUI를 호출하는 함수입니다.
def VideoViewer(rtsp_url):
    print("Launching VideoViewer with RTSP URL:", rtsp_url)
    # 여기에 실제 스트리밍 및 레코딩을 하는 GUI 구현 코드를 넣으세요.

class StreamSelector(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('스트리밍 주소 및 메뉴 선택')
        self.setGeometry(100, 100, 300, 200)

        layout = QVBoxLayout()

        # RTSP 주소 선택용 레이블과 드롭다운
        self.rtsp_label = QLabel('RTSP 주소 선택:', self)
        layout.addWidget(self.rtsp_label)

        self.rtsp_dropdown = QComboBox(self)
        self.rtsp_dropdown.addItems(['RTSP 주소 1', 'RTSP 주소 2'])
        layout.addWidget(self.rtsp_dropdown)

        # 메뉴 옵션 선택용 레이블과 드롭다운
        self.menu_label = QLabel('메뉴 옵션 선택:', self)
        layout.addWidget(self.menu_label)

        self.menu_dropdown = QComboBox(self)
        self.menu_dropdown.addItems(['메뉴 옵션 1', '메뉴 옵션 2'])
        layout.addWidget(self.menu_dropdown)

        # 시작 버튼
        self.start_button = QPushButton('시작', self)
        self.start_button.clicked.connect(self.start_streaming)
        layout.addWidget(self.start_button)

        self.setLayout(layout)

    def start_streaming(self):
        # 선택한 RTSP 주소와 메뉴 옵션 저장
        selected_rtsp = self.rtsp_dropdown.currentText()
        selected_menu = self.menu_dropdown.currentText()
        print(f"선택된 RTSP 주소: {selected_rtsp}")
        print(f"선택된 메뉴 옵션: {selected_menu}")

        # 사용자가 선택한 RTSP 주소를 통해 기존 스트리밍 GUI 호출
        VideoViewer(selected_rtsp)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    selector = StreamSelector()
    selector.show()
    sys.exit(app.exec_())
```

이 코드는 먼저 스트리밍 주소와 메뉴 옵션을 선택할 수 있는 간단한 인터페이스를 구성한 후, 시작 버튼을 누르면 선택된 RTSP 주소를 인자로 기존 스트리밍 GUI를 호출합니다. 필요에 따라 VideoViewer 함수 내부에 실제 스트리밍 및 레코딩 구현 코드를 추가하여 사용하시면 됩니다.

이 예제 코드를 참고하여 귀하의 프로젝트에 맞게 확장할 수 있습니다.

출처
