아래는 `self.cap.read()`로 AVI 파일을 프레임별로 읽어오고, 각 프레임의 오른쪽 상단에 현재 시간과 사용자 정의 텍스트를 오버레이하는 코드입니다. PyQt5와 OpenCV를 함께 사용하여 프레임을 처리하고 QLabel에 표시합니다.

---

## **코드**

```python
import sys
import cv2
import datetime
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import QTimer


class VideoPlayer(QMainWindow):
    def __init__(self, video_path):
        super().__init__()
        self.setWindowTitle("AVI Video Player with Text Overlay")
        self.setGeometry(100, 100, 800, 600)

        # Video capture setup
        self.cap = cv2.VideoCapture(video_path)
        if not self.cap.isOpened():
            print("Error: Cannot open video file.")
            sys.exit()

        # Timer for frame updates
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)

        # Main widget and layout
        self.central_widget = QWidget()
        self.layout = QVBoxLayout(self.central_widget)

        # QLabel for displaying the video frame
        self.video_label = QLabel()
        self.video_label.setAlignment(Qt.AlignCenter)
        self.layout.addWidget(self.video_label)

        self.setCentralWidget(self.central_widget)

        # Start the timer based on the video's FPS
        fps = int(self.cap.get(cv2.CAP_PROP_FPS))
        self.timer.start(1000 // fps)

    def update_frame(self):
        """Read the next frame from the video and display it with text overlay."""
        ret, frame = self.cap.read()

        if not ret:
            print("End of video.")
            self.timer.stop()
            return

        # Add text overlay to the frame
        frame = self.add_text_overlay(frame)

        # Convert OpenCV frame (BGR) to QImage (RGB)
        height, width, channel = frame.shape
        bytes_per_line = channel * width
        q_image = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGB888).rgbSwapped()

        # Convert QImage to QPixmap and set it to the QLabel
        pixmap = QPixmap.fromImage(q_image)
        self.video_label.setPixmap(pixmap)

    def add_text_overlay(self, frame):
        """Add current time and custom text to the top-right corner of the frame."""
        # Get current time and custom text
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        custom_text = "Your Custom Text Here"

        # Define text properties
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 1
        color = (0, 255, 0)  # Green color in BGR
        thickness = 2

        # Get frame dimensions
        height, width = frame.shape[:2]

        # Calculate positions for the text (right-aligned)
        text_size_time = cv2.getTextSize(current_time, font, font_scale, thickness)[0]
        text_size_custom = cv2.getTextSize(custom_text, font, font_scale, thickness)[0]

        x_time = width - text_size_time[0] - 10  # Align to the right with padding
        y_time = 30  # Top padding for the first line

        x_custom = width - text_size_custom[0] - 10  # Align to the right with padding
        y_custom = y_time + text_size_time[1] + 10  # Below the first line with spacing

        # Put the texts on the frame
        cv2.putText(frame, current_time, (x_time, y_time), font, font_scale, color, thickness, cv2.LINE_AA)
        cv2.putText(frame, custom_text, (x_custom, y_custom), font, font_scale, color, thickness, cv2.LINE_AA)

        return frame


if __name__ == "__main__":
    app = QApplication(sys.argv)
    video_path = "example.avi"  # Replace with your AVI file path
    player = VideoPlayer(video_path)
    player.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

### **1. OpenCV로 AVI 파일 읽기**
- `cv2.VideoCapture(video_path)`를 사용하여 AVI 파일을 읽습니다.
- `self.cap.read()`로 프레임을 읽어옵니다.

### **2. PyQt5 QLabel에 프레임 표시**
- OpenCV 프레임(BGR 형식)을 PyQt5에서 사용할 수 있도록 `QImage`로 변환합니다:
  ```python
  q_image = QImage(frame.data,
                   width,
                   height,
                   bytes_per_line,
                   QImage.Format_RGB888).rgbSwapped()
  ```
- 변환된 `QImage`를 `QPixmap`으로 설정하여 QLabel에 표시합니다:
  ```python
  pixmap = QPixmap.fromImage(q_image)
  self.video_label.setPixmap(pixmap)
  ```

### **3. 텍스트 오버레이**
- `add_text_overlay` 메서드에서 현재 시간과 사용자 정의 텍스트를 프레임의 오른쪽 상단에 추가합니다.
- OpenCV의 `cv2.putText`를 사용하여 텍스트를 추가합니다:
  ```python
  cv2.putText(frame, current_time, (x_time, y_time), font, font_scale, color, thickness, cv2.LINE_AA)
  ```

### **4. 타이머 기반 업데이트**
- PyQt5의 `QTimer`를 사용하여 일정 간격으로 `update_frame` 메서드를 호출합니다.
- FPS에 따라 타이머 간격을 설정합니다:
  ```python
  fps = int(self.cap.get(cv2.CAP_PROP_FPS))
  self.timer.start(1000 // fps)
  ```

---

## **결과**

1. AVI 파일이 프레임별로 읽혀지고 QLabel에 표시됩니다.
2. 각 프레임의 오른쪽 상단에 현재 시간과 사용자 정의 텍스트가 두 줄로 표시됩니다.
3. 동영상 재생이 끝나면 타이머가 멈추고 프로그램이 종료됩니다.

---

이 코드는 OpenCV와 PyQt5를 결합하여 동영상 위에 실시간으로 텍스트를 오버레이하는 방법을 제공합니다. 필요에 따라 텍스트 스타일이나 위치를 조정할 수 있습니다!

출처
