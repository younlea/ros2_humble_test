아래는 주어진 요구사항을 충족하는 **전체 코드**입니다. PyQt5와 OpenCV를 사용하여 AVI 파일을 열고, ROI를 선택하고, 선택된 영역을 2프레임마다 캡처하여 두 이미지를 위아래로 결합한 후 순서대로 저장하는 프로그램입니다.

```python
import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QFileDialog, QLabel, QVBoxLayout, QPushButton, QWidget
)
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from PyQt5.QtGui import QImage, QPixmap


class VideoThread(QThread):
    change_pixmap_signal = pyqtSignal(np.ndarray)

    def __init__(self, video_path):
        super().__init__()
        self.video_path = video_path
        self._run_flag = True

    def run(self):
        cap = cv2.VideoCapture(self.video_path)
        while self._run_flag and cap.isOpened():
            ret, frame = cap.read()
            if ret:
                self.change_pixmap_signal.emit(frame)
            else:
                break
        cap.release()

    def stop(self):
        self._run_flag = False
        self.wait()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("AVI Viewer with ROI Selection")
        self.setGeometry(100, 100, 800, 600)

        # UI Elements
        self.label = QLabel(self)
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setStyleSheet("background-color: black;")
        
        self.start_button = QPushButton("Open Video", self)
        self.start_button.clicked.connect(self.open_video)

        self.capture_button = QPushButton("Start Capturing", self)
        self.capture_button.setEnabled(False)
        self.capture_button.clicked.connect(self.start_capturing)

        layout = QVBoxLayout()
        layout.addWidget(self.label)
        layout.addWidget(self.start_button)
        layout.addWidget(self.capture_button)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        # Variables
        self.video_thread = None
        self.roi = None
        self.frame_count = 0
        self.image_index = 1

    def open_video(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Open AVI File", "", "AVI Files (*.avi);;All Files (*)", options=options
        )
        
        if file_path:
            if self.video_thread:
                self.video_thread.stop()
            
            self.video_thread = VideoThread(file_path)
            self.video_thread.change_pixmap_signal.connect(self.update_image)
            self.video_thread.start()
            self.capture_button.setEnabled(True)
    def update_image(self, frame):
        if self.roi is None:
            # ROI 선택
            cv2.imshow("Select ROI", frame)
            roi = cv2.selectROI("Select ROI", frame, fromCenter=False, showCrosshair=True)
            cv2.destroyWindow("Select ROI")
            if roi[2] > 0 and roi[3] > 0:
                self.roi = roi

        if self.roi is not None:
            x, y, w, h = map(int, self.roi)
            roi_frame = frame[y:y + h, x:x + w]

            # 화면에 ROI 영역 표시
            rgb_image = cv2.cvtColor(roi_frame, cv2.COLOR_BGR2RGB)
            h, w, ch = rgb_image.shape
            bytes_per_line = ch * w
            qt_image = QImage(rgb_image.data, w, h, bytes_per_line, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(qt_image)
            self.label.setPixmap(pixmap)

    def start_capturing(self):
        if self.video_thread:
            self.video_thread.stop()

        # AVI 파일 다시 열기
        cap = cv2.VideoCapture(self.video_thread.video_path)
        saved_images = []

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                break

            self.frame_count += 1

            # 2프레임마다 ROI 영역 저장
            if self.frame_count % 2 == 0 and self.roi is not None:
                x, y, w, h = map(int, self.roi)
                roi_frame = frame[y:y + h, x:x + w]
                saved_images.append(roi_frame)

                # 두 프레임을 위아래로 결합하여 저장
                if len(saved_images) == 2:
                    combined_image = np.vstack(saved_images)
                    save_path = f"{self.image_index}.jpg"
                    cv2.imwrite(save_path, combined_image)
                    print(f"Saved: {save_path}")
                    self.image_index += 1
                    saved_images = []

        cap.release()
        print("Capturing completed.")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())
```
```
코드 설명
	1.	AVI 파일 열기: `open_video` 메서드는 AVI 파일을 열고 비디오를 재생합니다.
	2.	ROI 선택: 첫 번째 프레임에서 `cv2.selectROI`를 사용하여 관심 영역(ROI)을 선택합니다.
	3.	프레임 표시: 선택된 ROI 영역만 화면에 표시되며 PyQt5의 QLabel 위젯에 렌더링됩니다.
	4.	ROI 캡처 및 저장:
	•	`start_capturing` 메서드는 비디오를 다시 열고 2프레임마다 ROI를 추출합니다.
	•	두 개의 연속된 프레임에서 ROI를 추출한 후 위아래로 결합(`np.vstack`)하여 JPG 파일로 저장합니다.
	•	저장된 파일은 “1.jpg”, “2.jpg”와 같은 이름으로 순차적으로 저장됩니다.
주요 의존성
	•	PyQt5: GUI 구성 및 이벤트 처리.
	•	OpenCV: AVI 파일 처리 및 ROI 선택.
이 코드는 PyQt5와 OpenCV가 설치된 Python 환경에서 실행 가능합니다. GUI를 통해 AVI 파일을 열고 ROI를 선택한 뒤 버튼 클릭으로 데이터 생성이 가능합니다.
```
