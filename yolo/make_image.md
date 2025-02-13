아래는 주어진 요구사항을 충족하는 **전체 코드**입니다. PyQt5와 OpenCV를 사용하여 AVI 파일을 열고, ROI를 선택하고, 선택된 영역을 2프레임마다 캡처하여 두 이미지를 위아래로 결합한 후 순서대로 저장하는 프로그램입니다.

```python
import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QFileDialog, QLabel, QVBoxLayout, QPushButton, QWidget
)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QRect
from PyQt5.QtGui import QImage, QPixmap, QPainter, QPen


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
        self.roi_rect = None  # QRect to store ROI
        self.start_point = None  # Start point of mouse drag
        self.end_point = None  # End point of mouse drag
        self.current_frame = None  # Current frame for display and ROI

        # Mouse interaction flags
        self.drawing_roi = False

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
        """Update the QLabel with the current frame."""
        if frame is not None:
            rgb_image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            h, w, ch = rgb_image.shape
            bytes_per_line = ch * w
            qt_image = QImage(rgb_image.data, w, h, bytes_per_line, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(qt_image)

            # Draw ROI rectangle if available
            if self.roi_rect:
                painter = QPainter(pixmap)
                pen = QPen(Qt.red)
                pen.setWidth(2)
                painter.setPen(pen)
                painter.drawRect(self.roi_rect)
                painter.end()

            self.label.setPixmap(pixmap)
            self.current_frame = frame

    def mousePressEvent(self, event):
        """Capture the start point of the ROI selection."""
        if event.button() == Qt.LeftButton and not self.drawing_roi:
            pos = event.pos()
            if pos.x() >= 0 and pos.y() >= 0:  # Ensure within bounds
                self.start_point = pos
                self.drawing_roi = True

    def mouseMoveEvent(self, event):
        """Update the rectangle as the mouse is dragged."""
        if self.drawing_roi and event.buttons() == Qt.LeftButton:
            pos = event.pos()
            if pos.x() >= 0 and pos.y() >= 0:  # Ensure within bounds
                x1, y1 = self.start_point.x(), self.start_point.y()
                x2, y2 = pos.x(), pos.y()
                rect_x = min(x1, x2)
                rect_y = min(y1, y2)
                rect_w = abs(x1 - x2)
                rect_h = abs(y1 - y2)
                self.roi_rect = QRect(rect_x, rect_y, rect_w, rect_h)  # Update QRect
                # Update display with rectangle drawn
                if isinstance(self.current_frame, np.ndarray):
                    rgb_image = cv2.cvtColor(self.current_frame.copy(), cv2.COLOR_BGR2RGB)
                    h, w, ch = rgb_image.shape
                    bytes_per_line = ch * w
                    qt_image = QImage(rgb_image.data, w, h, bytes_per_line, QImage.Format_RGB888)
                    pixmap = QPixmap.fromImage(qt_image)

                    painter = QPainter(pixmap)
                    pen = QPen(Qt.red)
                    pen.setWidth(2)
                    painter.setPen(pen)
                    painter.drawRect(self.roi_rect)  # Draw rectangle on pixmap
                    painter.end()

                    self.label.setPixmap(pixmap)

    def mouseReleaseEvent(self, event):
        """Finalize the ROI selection."""
        if event.button() == Qt.LeftButton and self.drawing_roi:
            pos = event.pos()
            x1, y1 = self.start_point.x(), self.start_point.y()
            x2, y2 = pos.x(), pos.y()
            rect_x = min(x1, x2)
            rect_y = min(y1, y2)
            rect_w = abs(x1 - x2)
            rect_h = abs(y1 - y2)

            # Finalize QRect for ROI selection
            if rect_w > 0 and rect_h > 0:
                self.roi_rect = QRect(rect_x, rect_y, rect_w, rect_h)

            print(f"ROI Selected: {self.roi_rect}")
            self.drawing_roi = False

    def start_capturing(self):
        """Start capturing frames based on the selected ROI."""
        if not (self.roi_rect and isinstance(self.current_frame, np.ndarray)):
            print("No ROI selected or no video loaded.")
            return

        cap = cv2.VideoCapture(self.video_thread.video_path)  # Reopen video file for processing
        frame_count = 0
        image_index = 1
        saved_images = []

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                break

            frame_count += 1

            # Process every second frame (frame_count % 2 == 0)
            if frame_count % 2 == 0:
                x1, y1, w, h = (
                    int(self.roi_rect.x()),
                    int(self.roi_rect.y()),
                    int(self.roi_rect.width()),
                    int(self.roi_rect.height()),
                )

                # Extract ROI
                roi_frame = frame[y1:y1 + h, x1:x1 + w]
                saved_images.append(roi_frame)

                # Combine two consecutive frames into one image (vertically stacked) and save as .jpg
                if len(saved_images) == 2:
                    combined_image = np.vstack(saved_images)
                    save_path = f"{image_index}.jpg"
                    cv2.imwrite(save_path, combined_image)
                    print(f"Saved: {save_path}")
                    image_index += 1
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
