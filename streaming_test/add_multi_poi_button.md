아래는 요청하신 대로 **버튼을 눌러 ROI를 선택한 후, 다시 버튼을 누르면 해당 ROI 값을 초기화(Reset)**하는 기능을 추가한 전체 코드입니다. 버튼을 클릭하면 기존 ROI 값을 초기화하거나 새롭게 선택하도록 동작합니다.

---

## **전체 코드**

```python
import sys
import cv2
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QWidget
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtGui import QImage, QPixmap


class VideoPlayer(QMainWindow):
    def __init__(self, video_path):
        super().__init__()
        self.setWindowTitle("AVI Video Player with ROI Selection and Reset")
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

        # ROI Buttons and storage
        self.roi_buttons = []
        self.roi_info = [None, None, None, None]  # Store ROI info for 4 regions
        self.current_roi_index = -1  # Track which ROI is being selected
        button_layout = QHBoxLayout()
        
        for i in range(4):
            button = QPushButton(f"Select ROI {i+1}")
            button.clicked.connect(lambda checked, idx=i: self.handle_roi_button(idx))
            self.roi_buttons.append(button)
            button_layout.addWidget(button)

        self.layout.addLayout(button_layout)
        self.setCentralWidget(self.central_widget)

        # Start the timer based on the video's FPS
        fps = int(self.cap.get(cv2.CAP_PROP_FPS))
        self.timer.start(1000 // fps)

    def handle_roi_button(self, idx):
        """Handle ROI button click: reset or start selecting."""
        if self.roi_info[idx] is not None:
            # Reset the ROI if it already exists
            print(f"Resetting ROI {idx+1}")
            self.roi_info[idx] = None
            self.roi_buttons[idx].setText(f"Select ROI {idx+1}")
            self.current_roi_index = -1  # Stop any ongoing selection
        else:
            # Start selecting a new ROI for this index
            print(f"Selecting ROI for button {idx+1}")
            self.current_roi_index = idx

    def mousePressEvent(self, event):
        """Capture the starting point of the ROI."""
        if event.button() == Qt.LeftButton and self.current_roi_index != -1:
            x = event.pos().x() - self.video_label.x()
            y = event.pos().y() - self.video_label.y()
            if x >= 0 and y >= 0:
                self.roi_start = (x, y)

    def mouseReleaseEvent(self, event):
        """Capture the ending point of the ROI and save it."""
        if event.button() == Qt.LeftButton and self.current_roi_index != -1:
            x_end = event.pos().x() - self.video_label.x()
            y_end = event.pos().y() - self.video_label.y()

            if x_end >= 0 and y_end >= 0 and hasattr(self, 'roi_start'):
                x_start, y_start = self.roi_start
                width = abs(x_end - x_start)
                height = abs(y_end - y_start)
                x_min = min(x_start, x_end)
                y_min = min(y_start, y_end)

                # Save the ROI info as (x, y, width, height)
                roi_data = (x_min, y_min, width, height)
                self.roi_info[self.current_roi_index] = roi_data

                # Update the button text with ROI info
                self.roi_buttons[self.current_roi_index].setText(
                    f"ROI {self.current_roi_index + 1}: {roi_data}"
                )

                print(f"ROI {self.current_roi_index + 1} set to: {roi_data}")

                # Reset current selection index
                self.current_roi_index = -1

    def update_frame(self):
        """Read the next frame from the video and display it."""
        ret, frame = self.cap.read()

        if not ret:
            print("End of video.")
            self.timer.stop()
            return

        # Draw ROIs on the frame
        for roi in self.roi_info:
            if roi is not None:
                x, y, w, h = roi
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Convert OpenCV frame (BGR) to QImage (RGB)
        height, width, channel = frame.shape
        bytes_per_line = channel * width
        q_image = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGB888).rgbSwapped()

        # Convert QImage to QPixmap and set it to the QLabel
        pixmap = QPixmap.fromImage(q_image)
        self.video_label.setPixmap(pixmap)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    video_path = "example.avi"  # Replace with your AVI file path
    player = VideoPlayer(video_path)
    player.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

### **1. `handle_roi_button` 메서드**
- 버튼 클릭 시 호출되며 두 가지 동작을 수행합니다:
  - **Reset**: 이미 설정된 ROI가 있는 경우 해당 값을 초기화하고 버튼 텍스트를 기본값으로 변경합니다.
    ```python
    if self.roi_info[idx] is not None:
        print(f"Resetting ROI {idx+1}")
        self.roi_info[idx] = None
        self.roi_buttons[idx].setText(f"Select ROI {idx+1}")
    ```
  - **Start Selection**: 선택 중인 ROI가 없으면 새로운 ROI를 선택할 준비를 합니다.
    ```python
    else:
        print(f"Selecting ROI for button {idx+1}")
        self.current_roi_index = idx
    ```

### **2. `mousePressEvent`와 `mouseReleaseEvent`**
- 마우스 클릭과 릴리즈 이벤트를 통해 선택된 영역의 시작점과 끝점을 캡처합니다.
- 선택된 영역은 `(x 시작 위치, y 시작 위치, 너비(width), 높이(height))` 형식으로 저장됩니다.

### **3. 버튼 업데이트**
- 선택이 완료되면 해당 버튼의 텍스트를 업데이트하여 현재 ROI 정보를 표시합니다:
  ```python
  roi_data = (x_min, y_min, width, height)
  self.roi_buttons[self.current_roi_index].setText(f"ROI {self.current_roi_index + 1}: {roi_data}")
  ```

---

## **결과**

1. AVI 영상이 QLabel에 표시됩니다.
2. 네 개의 버튼(`Select ROI 1`, ..., `Select ROI 4`)을 클릭하면 각각의 ROI를 선택하거나 초기화할 수 있습니다.
3. 선택된 ROI는 영상에 박스로 표시되며 버튼에 `(x 시작 위치, y 시작 위치, 너비(width), 높이(height))` 형식으로 표시됩니다.
4. 이미 선택된 버튼을 다시 누르면 해당 ROI 값이 초기화됩니다.

---

이 코드는 동적으로 여러 영역을 관리하고 필요에 따라 초기화할 수 있는 기능을 제공합니다. 필요에 따라 더 많은 확장을 추가할 수 있습니다!

출처
