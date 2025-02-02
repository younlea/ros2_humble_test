아래는 요청하신 기능을 구현한 코드입니다. 메인 화면 오른쪽 상단에 **시간 표시**와 **세로로 세 개의 알람 박스(녹색 또는 빨간색)**를 표시하며, 이상이 발생했을 때 박스 색상을 변경하는 기능을 제공합니다.

---

## **코드**

```python
import sys
import cv2
import datetime
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget
from PyQt5.QtGui import QImage, QPixmap, QPainter, QColor, QFont
from PyQt5.QtCore import Qt, QTimer


class VideoPlayer(QMainWindow):
    def __init__(self, video_path):
        super().__init__()
        self.setWindowTitle("AVI Video Player with Alarm Boxes")
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

        # Set the central widget
        self.setCentralWidget(self.central_widget)

        # Alarm box states (True for normal/green, False for abnormal/red)
        self.alarm_states = [True, True, True]  # Initial states: all normal

        # Alarm box labels
        self.alarm_labels = ["Dish", "Bart", "Conveyor Belt"]

        # Start the timer based on the video's FPS
        fps = int(self.cap.get(cv2.CAP_PROP_FPS))
        self.timer.start(1000 // fps)

    def update_frame(self):
        """Read the next frame from the video and display it with alarm boxes."""
        ret, frame = self.cap.read()

        if not ret:
            print("End of video.")
            self.timer.stop()
            return

        # Add text overlay (current time)
        frame = self.add_text_overlay(frame)

        # Draw alarm boxes on the frame
        frame = self.draw_alarm_boxes(frame)

        # Convert OpenCV frame (BGR) to QImage (RGB)
        height, width, channel = frame.shape
        bytes_per_line = channel * width
        q_image = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGB888).rgbSwapped()

        # Convert QImage to QPixmap and set it to the QLabel
        pixmap = QPixmap.fromImage(q_image)
        self.video_label.setPixmap(pixmap)

    def add_text_overlay(self, frame):
        """Add current time to the top-right corner of the frame."""
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 1
        color = (0, 255, 0)  # Green color in BGR
        thickness = 2

        text_size_time = cv2.getTextSize(current_time, font, font_scale, thickness)[0]
        
        x_time = frame.shape[1] - text_size_time[0] - 10  # Align to the right with padding
        y_time = 30  # Top padding for the first line

        cv2.putText(frame, current_time, (x_time, y_time), font, font_scale, color, thickness, cv2.LINE_AA)
        
        return frame

    def draw_alarm_boxes(self, frame):
        """Draw alarm boxes on the right side of the frame."""
        
        box_width = 200  # Width of each alarm box
        box_height = 50   # Height of each alarm box
        spacing = 10      # Spacing between boxes
        
        x_start = frame.shape[1] - box_width - 10  # Align to the right with padding
        
        for i in range(len(self.alarm_states)):
            y_start = 50 + i * (box_height + spacing)  # Calculate y position for each box
            
            # Determine box color based on state (green for normal, red for abnormal)
            color = (0, 255, 0) if self.alarm_states[i] else (0, 0, 255)  # Green or Red
            
            # Draw the rectangle (alarm box)
            cv2.rectangle(frame,
                          (x_start, y_start),
                          (x_start + box_width, y_start + box_height),
                          color,
                          -1)  # -1 fills the rectangle
            
            # Add text inside the box
            text_position = (x_start + 10, y_start + int(box_height / 2) + 5)
            cv2.putText(frame,
                        self.alarm_labels[i],
                        text_position,
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (255, 255, 255),  # White text color
                        2,
                        cv2.LINE_AA)
        
        return frame

    def toggle_alarm_state(self, index):
        """Toggle the state of a specific alarm box."""
        if index < len(self.alarm_states):
            self.alarm_states[index] = not self.alarm_states[index]


if __name__ == "__main__":
    app = QApplication(sys.argv)
    video_path = "example.avi"  # Replace with your AVI file path
    player = VideoPlayer(video_path)
    player.show()

    # Example: Toggle alarm states after a few seconds using a timer or external event.
    from PyQt5.QtCore import QTimer

    def toggle_example():
        player.toggle_alarm_state(0)  # Toggle the first alarm state (Dish)
    
    example_timer = QTimer()
    example_timer.timeout.connect(toggle_example)
    example_timer.start(3000)  # Toggle every 3 seconds

    sys.exit(app.exec_())
```

---

## **코드 설명**

### **1. 알람 박스 상태 관리**
- `self.alarm_states`: 각 알람 박스의 상태를 저장합니다 (`True`는 정상 상태로 녹색 표시).
- `self.alarm_labels`: 각 박스의 이름을 저장합니다 (`Dish`, `Bart`, `Conveyor Belt`).

### **2. 알람 박스 그리기**
- `draw_alarm_boxes` 함수에서 OpenCV의 `cv2.rectangle`로 각 박스를 그립니다.
- 상태에 따라 색상을 변경합니다:
   ```python
   color = (0, 255, 0) if self.alarm_states[i] else (0, 0, 255) 
   ```
- 텍스트는 박스 내부에 표시됩니다.

### **3. 알람 상태 토글**
- `toggle_alarm_state` 메서드를 사용하여 특정 알람 박스의 상태를 변경합니다.

### **4. 예제: 상태 변경**
- `QTimer`를 사용하여 주기적으로 알람 상태를 변경하는 예제를 추가했습니다:
   ```python
   def toggle_example():
       player.toggle_alarm_state(0) 
   ```

---

## **결과**

1. 오른쪽 상단에 현재 시간이 표시됩니다.
2. 오른쪽에 세 개의 알람 박스가 세로로 배치됩니다.
3. 정상 상태일 때는 녹색 박스로 표시되며 이상이 발생하면 빨간색으로 변경됩니다.
4. 텍스트는 각각 `Dish`, `Bart`, `Conveyor Belt`로 표시됩니다.

---

이 코드는 동영상 프레임 위에 텍스트와 알람 박스를 오버레이하는 기능을 제공합니다. 필요에 따라 알람 상태를 외부 이벤트나 조건에 따라 동적으로 변경할 수 있습니다!

출처
