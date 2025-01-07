아래는 Python을 사용하여 AVI 파일을 열고, 영상을 재생하며, 영상 컨트롤 버튼 (+10초, +5초, 정지/시작, -5초, -10초)을 추가하는 프로그램 코드입니다.    
이 코드는 `tkinter`를 사용하여 파일 선택 팝업창과 버튼 UI를 구현하고, `OpenCV`를 사용하여 영상을 재생합니다.

## **전체 코드**

아래는 앞서 제공한 코드의 나머지 부분과 전체 코드를 완성한 것입니다. 이 코드에는 AVI 파일을 열고, 영상을 재생하며, 영상 컨트롤 버튼을 사용하는 기능이 모두 포함되어 있습니다.

## **완성된 코드**

```python
import cv2
import tkinter as tk
from tkinter import filedialog
from tkinter import ttk
from PIL import Image, ImageTk  # For displaying frames in tkinter

class VideoPlayer:
    def __init__(self, root):
        self.root = root
        self.root.title("AVI Video Player")
        
        # Video variables
        self.video_path = None
        self.cap = None
        self.is_playing = False
        self.current_frame = 0
        self.total_frames = 0
        self.fps = 0
        
        # Create UI components
        self.create_ui()
    
    def create_ui(self):
        # File open button
        open_button = ttk.Button(self.root, text="Open Video", command=self.open_video)
        open_button.pack(pady=10)
        
        # Canvas for video display
        self.canvas = tk.Canvas(self.root, width=640, height=480, bg="black")
        self.canvas.pack()
        
        # Control buttons
        control_frame = tk.Frame(self.root)
        control_frame.pack(pady=10)

        ttk.Button(control_frame, text="-10s", command=lambda: self.skip_video(-10)).grid(row=0, column=0, padx=5)
        ttk.Button(control_frame, text="-5s", command=lambda: self.skip_video(-5)).grid(row=0, column=1, padx=5)
        
        self.play_button = ttk.Button(control_frame, text="Play", command=self.toggle_playback)
        self.play_button.grid(row=0, column=2, padx=5)
        
        ttk.Button(control_frame, text="+5s", command=lambda: self.skip_video(5)).grid(row=0, column=3, padx=5)
        ttk.Button(control_frame, text="+10s", command=lambda: self.skip_video(10)).grid(row=0, column=4, padx=5)
    
    def open_video(self):
        # Open file dialog to select a video file
        self.video_path = filedialog.askopenfilename(filetypes=[("AVI files", "*.avi")])
        
        if not self.video_path:
            return
        
        # Open the video file with OpenCV
        self.cap = cv2.VideoCapture(self.video_path)
        
        if not self.cap.isOpened():
            print("Error: Cannot open video file.")
            return
        
        # Get video properties
        self.total_frames = int(self.cap.get(cv2.CAP_PROP_FRAME_COUNT))
        self.fps = int(self.cap.get(cv2.CAP_PROP_FPS))
        
        # Reset playback state
        self.current_frame = 0
        self.is_playing = False
        self.play_button.config(text="Play")
    
    def toggle_playback(self):
        if not self.cap:
            return
        
        if self.is_playing:
            # Pause the video
            self.is_playing = False
            self.play_button.config(text="Play")
        else:
            # Start or resume the video
            self.is_playing = True
            self.play_button.config(text="Pause")
            self.play_video()
    
    def play_video(self):
        if not self.cap:
            return
        
        while self.is_playing:
            ret, frame = self.cap.read()
            
            if not ret:
                print("End of video.")
                break
            
            # Display the frame on the canvas using PIL for compatibility with tkinter
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            img = Image.fromarray(frame_rgb)
            imgtk = ImageTk.PhotoImage(image=img)
            
            # Update canvas with the new frame
            self.canvas.create_image(0, 0, anchor=tk.NW, image=imgtk)
            self.root.update_idletasks()
            self.root.update()
            
            # Increment current frame index and delay based on FPS
            self.current_frame += 1
            delay = int(1000 / self.fps)  # Delay in milliseconds for smooth playback
            
            cv2.waitKey(delay)

    def skip_video(self, seconds):
        if not (self.cap and self.fps):
            return
        
        target_frame = int(self.current_frame + seconds * self.fps)
        
        if target_frame < 0:
            target_frame = 0
        elif target_frame >= self.total_frames:
            target_frame = self.total_frames - 1
        
        # Set the new frame position and update playback state
        self.cap.set(cv2.CAP_PROP_POS_FRAMES, target_frame)
        ret, frame = self.cap.read()
        
        if ret:
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            img = Image.fromarray(frame_rgb)
            imgtk = ImageTk.PhotoImage(image=img)
            
            # Update canvas with the skipped frame
            self.canvas.create_image(0, 0, anchor=tk.NW, image=imgtk)
        
        # Update current frame index to reflect the skip action
        self.current_frame = target_frame

# Main application loop
if __name__ == "__main__":
    root = tk.Tk()
    app = VideoPlayer(root)
    root.mainloop()
```

---

## **코드 설명**

1. **파일 선택 및 열기**:
   - `filedialog.askopenfilename()`을 사용하여 AVI 파일을 선택합니다.
   - OpenCV의 `cv2.VideoCapture()`로 동영상을 열고 속성(`CAP_PROP_FRAME_COUNT`, `CAP_PROP_FPS`)을 가져옵니다.

2. **영상 재생**:
   - `play_video()` 함수에서 프레임을 읽어와 `tkinter.Canvas`에 표시합니다.
   - `PIL.Image`와 `ImageTk.PhotoImage`를 사용하여 OpenCV 프레임을 tkinter에서 렌더링할 수 있도록 변환합니다.

3. **컨트롤 버튼**:
   - `+10s`, `+5s`, `-5s`, `-10s`: 현재 프레임에서 지정된 초만큼 이동합니다.
   - `toggle_playback()`: 재생/일시정지를 전환합니다.

4. **UI 구성**:
   - `tkinter`를 사용하여 간단한 버튼과 캔버스를 배치합니다.

---

## **실행 방법**

1. 위 코드를 Python 파일로 저장합니다(예: `video_player.py`).
2. 터미널에서 필요한 라이브러리를 설치합니다:

   ```bash
   pip install opencv-python-headless pillow tk
   ```

3. 프로그램을 실행하고 AVI 파일을 선택하면 동영상이 재생됩니다. 버튼으로 영상을 제어할 수 있습니다.


# PyQt5 사용.   
아래는 PyQt5를 사용하여 AVI 파일을 열고, 재생하며, 영상 컨트롤 버튼 (+10초, +5초, 정지/시작, -5초, -10초)을 포함하는 동영상 플레이어 프로그램의 코드입니다. 이 코드는 `QMediaPlayer`와 `QVideoWidget`을 사용하여 구현되었습니다.

## **PyQt5 코드**

```python
import sys
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout,
    QPushButton, QFileDialog, QWidget, QSlider
)
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtCore import Qt, QUrl


class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 AVI Video Player")
        self.setGeometry(100, 100, 800, 600)

        # Initialize media player and video widget
        self.media_player = QMediaPlayer(None, QMediaPlayer.VideoSurface)
        self.video_widget = QVideoWidget()

        # Create UI components
        self.create_ui()

        # Set up media player
        self.media_player.setVideoOutput(self.video_widget)
        self.media_player.positionChanged.connect(self.update_slider)
        self.media_player.durationChanged.connect(self.update_slider_range)

    def create_ui(self):
        # Layouts
        central_widget = QWidget(self)
        main_layout = QVBoxLayout()
        control_layout = QHBoxLayout()

        # Video display area
        main_layout.addWidget(self.video_widget)

        # Control buttons
        btn_open = QPushButton("Open")
        btn_open.clicked.connect(self.open_file)
        
        btn_play_pause = QPushButton("Play")
        btn_play_pause.clicked.connect(self.play_pause_video)
        
        btn_back_10s = QPushButton("-10s")
        btn_back_10s.clicked.connect(lambda: self.skip_video(-10))
        
        btn_back_5s = QPushButton("-5s")
        btn_back_5s.clicked.connect(lambda: self.skip_video(-5))
        
        btn_forward_5s = QPushButton("+5s")
        btn_forward_5s.clicked.connect(lambda: self.skip_video(5))
        
        btn_forward_10s = QPushButton("+10s")
        btn_forward_10s.clicked.connect(lambda: self.skip_video(10))

        # Add buttons to control layout
        control_layout.addWidget(btn_open)
        control_layout.addWidget(btn_back_10s)
        control_layout.addWidget(btn_back_5s)
        control_layout.addWidget(btn_play_pause)
        control_layout.addWidget(btn_forward_5s)
        control_layout.addWidget(btn_forward_10s)

        # Slider for video position
        self.position_slider = QSlider(Qt.Horizontal)
        self.position_slider.sliderMoved.connect(self.set_position)

        # Add slider and controls to main layout
        main_layout.addWidget(self.position_slider)
        main_layout.addLayout(control_layout)

        # Set layout to central widget
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

    def open_file(self):
        file_name, _ = QFileDialog.getOpenFileName(
            self, "Open Video", "", "Video Files (*.avi *.mp4 *.mkv)"
        )
        
        if file_name:
            self.media_player.setMedia(QMediaContent(QUrl.fromLocalFile(file_name)))
            self.media_player.play()

    def play_pause_video(self):
        if self.media_player.state() == QMediaPlayer.PlayingState:
            self.media_player.pause()
            sender = self.sender()
            sender.setText("Play")
            
        else:
            self.media_player.play()
            sender = self.sender()
            sender.setText("Pause")

    def skip_video(self, seconds):
        current_position = self.media_player.position()
        new_position = current_position + (seconds * 1000)  # Convert seconds to milliseconds
        
        if new_position < 0:
            new_position = 0
        
        if new_position > self.media_player.duration():
            new_position = self.media_player.duration()
        
        self.media_player.setPosition(new_position)

    def update_slider(self, position):
        """Update slider position based on video playback position."""
        self.position_slider.setValue(position)

    def update_slider_range(self, duration):
        """Update the slider range when the video duration changes."""
        self.position_slider.setRange(0, duration)

    def set_position(self, position):
        """Set video playback position when slider is moved."""
        self.media_player.setPosition(position)


# Main application loop
if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

1. **`QMediaPlayer`와 `QVideoWidget`**:
   - `QMediaPlayer`는 동영상을 재생하고 제어하는 데 사용됩니다.
   - `QVideoWidget`은 동영상을 화면에 출력하는 위젯입니다.

2. **컨트롤 버튼**:
   - `Play/Pause`: 동영상 재생 및 일시정지 전환.
   - `+10s`, `+5s`, `-5s`, `-10s`: 동영상의 재생 위치를 앞뒤로 이동.

3. **슬라이더**:
   - 동영상의 현재 재생 위치를 표시하며 슬라이더를 이동하여 재생 위치를 변경할 수 있습니다.

4. **파일 열기**:
   - `QFileDialog`를 사용하여 AVI 파일을 선택하고 재생합니다.

---

## **실행 방법**

1. 위 코드를 Python 파일로 저장합니다(예: `pyqt_video_player.py`).
2. 필요한 라이브러리를 설치합니다:

   ```bash
   pip install pyqt5
   ```

3. 프로그램을 실행하고 AVI 파일을 선택하면 동영상이 재생됩니다. 컨트롤 버튼과 슬라이더를 사용하여 영상을 제어할 수 있습니다.

출처
[1] PyQt5 video widgets - Python https://pythonprogramminglanguage.com/pyqt5-video-widget/
[2] PyQt5 기반 동영상 플레이어앱 만들기 - 오션코딩학원 https://oceancoding.blogspot.com/2020/07/blog-post_22.html
[3] how to read video data from memory use pyqt5 - Stack Overflow https://stackoverflow.com/questions/49215830/how-to-read-video-data-from-memory-use-pyqt5
[4] Video Player with Qt5 - Python Forum https://python-forum.io/thread-1150.html
[5] PyQt5 VideoPlayer - GitHub Gist https://gist.github.com/Axel-Erfurt/af8bc3ff7dc11809b5ed3710af915b13?permalink_comment_id=2701220
[6] PyQt5 - open QMediaplayer in new window and play video https://stackoverflow.com/questions/53321617/pyqt5-open-qmediaplayer-in-new-window-and-play-video
[7] I can't play MP4 files - Python Forum https://python-forum.io/thread-34130.html
[8] Player Example - Qt for Python - Qt Documentation https://doc.qt.io/qtforpython-6/examples/example_multimedia_player.html
[9] [PyQt]QMediaPlayer로 Video Player만들기 (not use Pixmap) https://venypark.tistory.com/24
[10] Resizing a constant Video stream from OpenCV to PyQt5 ... - Qt Forum https://forum.qt.io/topic/84893/resizing-a-constant-video-stream-from-opencv-to-pyqt5-with-qpainter-in-a-qwidget-using-a-raspberry-pi

-------------
install 이후에도 멀티미디어가 동작을 하지 않는다.     
sudo apt update     
sudo apt install python3-pyqt5.qtmultimedia.    

The error message **"no service found for - 'org.qt-project.qt.mediaplayer'"** typically occurs when the required backend or plugins for the `QtMultimedia` module are missing or not properly configured. This issue is common on systems where multimedia dependencies (like GStreamer or Qt plugins) are not installed. Below are step-by-step solutions to resolve this issue:

---

## **1. Install Required Multimedia Plugins**

### **Linux**
On Linux systems, ensure the necessary QtMultimedia plugins and GStreamer libraries are installed:

```bash
sudo apt update
sudo apt install libqt5multimedia5-plugins
sudo apt install gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav
```

These packages provide the necessary backend for multimedia playback in PyQt5.

### **Windows**
For Windows, ensure that the `libgstmediaplayer.so` plugin is available in your PyQt5 installation. If it's missing, you can try reinstalling PyQt5:

```bash
pip uninstall PyQt5 PyQt5-sip
pip install PyQt5 PyQt5-sip
```

Additionally, ensure that you have a GStreamer installation on your system. Download and install GStreamer from its official website, and make sure to add it to your system's PATH.

---

## **2. Verify QtMultimedia Installation**

Run the following Python script to check if `PyQt5.QtMultimedia` and its widgets are properly installed:

```python
from PyQt5.QtMultimedia import QMediaPlayer
from PyQt5.QtMultimediaWidgets import QVideoWidget

print("QtMultimedia is installed and working!")
```

If this script raises an error, reinstall the `python3-pyqt5.qtmultimedia` package:

### **Linux**
```bash
sudo apt install python3-pyqt5.qtmultimedia
```

### **Windows/macOS**
Reinstall using pip:
```bash
pip install PyQt5.QtMultimedia
```

---

## **3. Debug Missing Plugins**

If the error persists, enable plugin debugging to identify missing libraries or plugins:

### **Linux**
Set the `QT_DEBUG_PLUGINS` environment variable before running your script:
```bash
export QT_DEBUG_PLUGINS=1
python your_script.py
```

### **Windows**
Run your script in a terminal with the debug flag:
```cmd
set QT_DEBUG_PLUGINS=1
python your_script.py
```

This will output detailed information about which plugins are missing or failing to load.

---

## **4. Manually Link Missing Plugins**

If the debug output indicates a specific missing file (e.g., `libgstmediaplayer.so`), you can manually link it:

### Example (Linux):
```bash
sudo ln -s /usr/lib/x86_64-linux-gnu/qt5/plugins/mediaservice/libgstmediaplayer.so /usr/local/lib/python3.x/dist-packages/PyQt5/Qt/plugins/mediaservice/libgstmediaplayer.so
```

Replace `/usr/lib/x86_64-linux-gnu/qt5/plugins/mediaservice/` with the correct path on your system.

---

## **5. Use Alternative Multimedia Frameworks**

If resolving this issue proves too complex, consider using an alternative Python library for video playback, such as `opencv-python` or `pygame`. These libraries do not rely on QtMultimedia and are easier to set up.

---

## **6. Example Code for Testing**

Here is a simple PyQt5 multimedia player code snippet to test after resolving dependencies:

```python
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QPushButton, QWidget
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtCore import QUrl

class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt Video Player")
        self.setGeometry(100, 100, 800, 600)

        # Media player setup
        self.media_player = QMediaPlayer(None, QMediaPlayer.VideoSurface)
        self.video_widget = QVideoWidget()

        # Layout setup
        layout = QVBoxLayout()
        layout.addWidget(self.video_widget)

        # Play button setup
        play_button = QPushButton("Play")
        play_button.clicked.connect(self.play_video)
        layout.addWidget(play_button)

        # Main widget setup
        widget = QWidget()
        widget.setLayout(layout)
        self.setCentralWidget(widget)

        # Connect media player to video widget
        self.media_player.setVideoOutput(self.video_widget)

    def play_video(self):
        file_url = "path/to/your/video.avi"  # Replace with your video file path
        media_content = QMediaContent(QUrl.fromLocalFile(file_url))
        self.media_player.setMedia(media_content)
        self.media_player.play()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```

---

By following these steps and ensuring all dependencies are properly installed, you should be able to resolve the "no service found for - 'org.qt-project.qt.mediaplayer'" error and successfully run multimedia applications using PyQt5.

출처
[1] Python How To Create Media Player In PyQt5 - YouTube https://www.youtube.com/watch?v=a6_5vkxLwAw
[2] How to install pyqt5 in jetson xavier - NVIDIA Developer Forums https://forums.developer.nvidia.com/t/how-to-install-pyqt5-in-jetson-xavier/79554
[3] Error: No module named 'PyQt5.QtMultimedia' - Stack Overflow https://stackoverflow.com/questions/61355187/error-no-module-named-pyqt5-qtmultimedia
[4] Plugin cannot be loaded for module “QtMultimedia ... - Qt Forum https://forum.qt.io/topic/109419/plugin-cannot-be-loaded-for-module-qtmultimedia-cannot-load-library-declarative_multimedia-dll
[5] Qt Multimedia 5.15.18 - Qt Documentation https://doc.qt.io/qt-5/qtmultimedia-index.html
[6] [PyQt] QtMultimedia 사용해 영상 재생하기 - 개발일지 - 티스토리 https://giveme-happyending.tistory.com/150
[7] Missing library for PyQt 5.11 (Broken QtMultimedia module) #211 https://github.com/pypa/packaging-problems/issues/211
[8] How To Install / Download PyQt5 -- 2024 - YouTube https://www.youtube.com/watch?v=aEUR-ngeXNA
[9] How to install module "QtMultimedia" on Windows - Qt Forum https://forum.qt.io/topic/124038/how-to-install-module-qtmultimedia-on-windows
[10] pyqt5 qtmultimedia 패키지 설치 - MakingRobot - 티스토리 https://makingrobot.tistory.com/145




------------
ROI영역 추가해서 아래 부분에 보여지기.   
아래는 PyQt5를 사용하여 메인 뷰어와 두 개의 작은 뷰어를 구현하고, 드래그로 ROI(Region of Interest) 영역을 설정하여 왼쪽 하단 뷰어에 선택한 영역을 표시하고, 오른쪽 하단 뷰어에 해당 영역의 엣지 검출(후처리) 결과를 표시하는 코드입니다. OpenCV를 사용하여 이미지 처리를 수행합니다.

---

## **코드**

```python
import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout,
    QPushButton, QWidget, QLabel, QFileDialog
)
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtCore import QUrl, Qt, QRect
from PyQt5.QtGui import QPixmap, QImage


class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 Video Player with ROI")
        self.setGeometry(100, 100, 1200, 800)

        # Media player setup
        self.media_player = QMediaPlayer(None, QMediaPlayer.VideoSurface)
        self.video_widget = QVideoWidget()

        # ROI variables
        self.roi_start = None
        self.roi_end = None
        self.roi_rect = None

        # Create UI components
        self.create_ui()

        # Connect media player to video widget
        self.media_player.setVideoOutput(self.video_widget)

    def create_ui(self):
        main_layout = QVBoxLayout()
        
        # Main video display (top)
        main_layout.addWidget(self.video_widget)

        # Bottom layout for small viewers and controls
        bottom_layout = QHBoxLayout()

        # Left viewer (ROI display)
        self.left_viewer = QLabel("ROI Viewer")
        self.left_viewer.setFixedSize(400, 300)
        self.left_viewer.setStyleSheet("background-color: black;")
        bottom_layout.addWidget(self.left_viewer)

        # Right viewer (Processed display)
        self.right_viewer = QLabel("Processed Viewer")
        self.right_viewer.setFixedSize(400, 300)
        self.right_viewer.setStyleSheet("background-color: black;")
        bottom_layout.addWidget(self.right_viewer)

        # Control buttons
        control_layout = QVBoxLayout()
        
        btn_open = QPushButton("Open Video")
        btn_open.clicked.connect(self.open_file)
        
        btn_play_pause = QPushButton("Play")
        btn_play_pause.clicked.connect(self.play_pause_video)
        
        control_layout.addWidget(btn_open)
        control_layout.addWidget(btn_play_pause)
        
        bottom_layout.addLayout(control_layout)

        main_layout.addLayout(bottom_layout)

        # Set layout to central widget
        central_widget = QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

    def open_file(self):
        file_name, _ = QFileDialog.getOpenFileName(
            self, "Open Video", "", "Video Files (*.avi *.mp4 *.mkv)"
        )
        
        if file_name:
            self.media_player.setMedia(QMediaContent(QUrl.fromLocalFile(file_name)))
            self.media_player.play()

    def play_pause_video(self):
        if self.media_player.state() == QMediaPlayer.PlayingState:
            self.media_player.pause()
            sender = self.sender()
            sender.setText("Play")
            
        else:
            self.media_player.play()
            sender = self.sender()
            sender.setText("Pause")

    def mousePressEvent(self, event):
        """Capture the starting point of the ROI."""
        if event.button() == Qt.LeftButton and event.y() < self.video_widget.height():
            self.roi_start = event.pos()

    def mouseReleaseEvent(self, event):
        """Capture the ending point of the ROI and process the region."""
        if event.button() == Qt.LeftButton and event.y() < self.video_widget.height():
            self.roi_end = event.pos()
            
            # Calculate ROI rectangle
            x1, y1 = min(self.roi_start.x(), self.roi_end.x()), min(self.roi_start.y(), self.roi_end.y())
            x2, y2 = max(self.roi_start.x(), self.roi_end.x()), max(self.roi_start.y(), self.roi_end.y())
            self.roi_rect = QRect(x1, y1, x2 - x1, y2 - y1)

            # Process ROI from current frame
            if hasattr(self.media_player, 'current_frame'):
                frame = getattr(self.media_player, 'current_frame', None)
                if frame is not None:
                    roi_frame = frame[y1:y2, x1:x2]
                    processed_frame = cv2.Canny(roi_frame, 50, 150)  # Edge detection

                    # Display ROI and processed frames in viewers
                    if roi_frame.size > 0:
                        roi_pixmap = self.convert_cv_to_pixmap(roi_frame)
                        processed_pixmap = self.convert_cv_to_pixmap(processed_frame)

                        if roi_pixmap:
                            self.left_viewer.setPixmap(roi_pixmap.scaled(
                                self.left_viewer.width(), 
                                self.left_viewer.height(), 
                                Qt.KeepAspectRatio))

                        if processed_pixmap:
                            self.right_viewer.setPixmap(processed_pixmap.scaled(
                                self.right_viewer.width(), 
                                self.right_viewer.height(), 
                                Qt.KeepAspectRatio))

    def convert_cv_to_pixmap(self, cv_img):
        """Convert OpenCV image to QPixmap."""
        height, width = cv_img.shape[:2]
        
        if len(cv_img.shape) == 3:  # Color image (BGR to RGB)
            cv_img_rgb = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
            qimg = QImage(cv_img_rgb.data, width, height, QImage.Format_RGB888)
        
        else:  # Grayscale image
            qimg = QImage(cv_img.data, width, height, width, QImage.Format_Grayscale8)

        return QPixmap.fromImage(qimg)


# Main application loop
if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

### **기능 구현**
1. **메인 뷰어**:
   - `QVideoWidget`을 사용하여 동영상을 재생합니다.
   - 마우스 드래그 이벤트를 통해 ROI를 설정합니다.

2. **하단 뷰어**:
   - 왼쪽 하단: 선택한 ROI 영역을 표시합니다.
   - 오른쪽 하단: ROI 영역에 대해 OpenCV로 엣지 검출(`cv2.Canny`)을 수행한 결과를 표시합니다.

3. **ROI 설정**:
   - `mousePressEvent`와 `mouseReleaseEvent`에서 드래그 시작 및 끝 좌표를 캡처하여 ROI 영역을 계산합니다.
   - 현재 프레임에서 해당 영역을 추출하고 처리합니다.

4. **OpenCV와 PyQt5 연동**:
   - OpenCV 이미지를 PyQt5에서 사용할 수 있도록 `QPixmap`으로 변환합니다.

---

## **실행 방법**

1. 위 코드를 Python 파일로 저장합니다(예: `pyqt5_roi_video_player.py`).
2. 필요한 라이브러리를 설치합니다:

   ```bash
   pip install pyqt5 opencv-python-headless numpy
   ```

3. 프로그램을 실행하고 동영상을 선택한 후 마우스로 드래그하여 ROI를 설정하면 하단 뷰어에 결과가 표시됩니다.

출처

------------      
최종.       

아래는 요청하신 기능을 완전히 구현한 코드입니다. ROI를 드래그하는 동안 메인 뷰어에 실시간으로 박스를 업데이트하고, ROI가 설정된 이후에도 해당 박스가 계속 표시됩니다.

---

## **완성된 코드**

```python
import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout,
    QPushButton, QWidget, QLabel, QFileDialog
)
from PyQt5.QtCore import QUrl, Qt, QRect, QTimer
from PyQt5.QtGui import QPixmap, QImage


class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 Video Player with ROI")
        self.setGeometry(100, 100, 1200, 800)

        # Media player setup
        self.video_widget = QLabel()  # Custom QLabel for drawing ROI
        self.video_widget.setStyleSheet("background-color: black;")

        # ROI variables
        self.roi_start = None
        self.roi_end = None
        self.roi_rect = None
        self.current_frame = None  # Store the current frame for processing

        # Timer for updating frames in the main viewer
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)

        # Create UI components
        self.create_ui()

    def create_ui(self):
        main_layout = QVBoxLayout()
        
        # Main video display (top)
        main_layout.addWidget(self.video_widget)

        # Bottom layout for small viewers and controls
        bottom_layout = QHBoxLayout()

        # Left viewer (ROI display)
        self.left_viewer = QLabel("ROI Viewer")
        self.left_viewer.setFixedSize(400, 300)
        self.left_viewer.setStyleSheet("background-color: black;")
        bottom_layout.addWidget(self.left_viewer)

        # Right viewer (Processed display)
        self.right_viewer = QLabel("Processed Viewer")
        self.right_viewer.setFixedSize(400, 300)
        self.right_viewer.setStyleSheet("background-color: black;")
        bottom_layout.addWidget(self.right_viewer)

        # Control buttons
        control_layout = QVBoxLayout()
        
        btn_open = QPushButton("Open Video")
        btn_open.clicked.connect(self.open_file)
        
        btn_play_pause = QPushButton("Play")
        btn_play_pause.clicked.connect(self.play_pause_video)
        
        control_layout.addWidget(btn_open)
        control_layout.addWidget(btn_play_pause)
        
        bottom_layout.addLayout(control_layout)

        main_layout.addLayout(bottom_layout)

        # Set layout to central widget
        central_widget = QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

    def open_file(self):
        file_name, _ = QFileDialog.getOpenFileName(
            self, "Open Video", "", "Video Files (*.avi *.mp4 *.mkv)"
        )
        
        if file_name:
            # Open video with OpenCV to extract frames manually
            self.cap = cv2.VideoCapture(file_name)
            if not self.cap.isOpened():
                print("Error: Cannot open video.")
                return
            
            # Start the timer to update frames in the main viewer
            self.timer.start(30)  # Adjust interval based on video FPS

    def play_pause_video(self):
        if hasattr(self, 'cap') and self.cap.isOpened():
            if self.timer.isActive():
                self.timer.stop()
                sender = self.sender()
                sender.setText("Play")
            else:
                self.timer.start(30)
                sender = self.sender()
                sender.setText("Pause")

    def update_frame(self):
        """Update the main viewer with the current frame."""
        ret, frame = self.cap.read()
        
        if not ret:
            print("End of video.")
            self.timer.stop()
            return

        # Store the current frame for ROI processing
        self.current_frame = frame.copy()

        # Draw ROI rectangle if it exists
        if self.roi_rect:
            x1, y1, x2, y2 = (self.roi_rect.x(), self.roi_rect.y(),
                              self.roi_rect.x() + self.roi_rect.width(),
                              self.roi_rect.y() + self.roi_rect.height())
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

        # Convert frame to QPixmap and display it in the main viewer
        pixmap = self.convert_cv_to_pixmap(frame)
        if pixmap:
            self.video_widget.setPixmap(pixmap.scaled(
                self.video_widget.width(),
                self.video_widget.height(),
                Qt.KeepAspectRatio))

    def mousePressEvent(self, event):
        """Capture the starting point of the ROI."""
        if event.button() == Qt.LeftButton and event.y() < self.video_widget.height():
            x_offset = event.pos().x() - self.video_widget.x()
            y_offset = event.pos().y() - self.video_widget.y()
            if x_offset >= 0 and y_offset >= 0:
                self.roi_start = (x_offset, y_offset)

    def mouseMoveEvent(self, event):
        """Update the ROI rectangle as the mouse moves."""
        if event.buttons() == Qt.LeftButton and event.y() < self.video_widget.height():
            x_offset = event.pos().x() - self.video_widget.x()
            y_offset = event.pos().y() - self.video_widget.y()
            if x_offset >= 0 and y_offset >= 0 and self.roi_start:
                x1, y1 = self.roi_start
                x2, y2 = x_offset, y_offset
                rect_frame = cv2.rectangle(
                    np.copy(self.current_frame), (x1, y1), (x2, y2), (0, 255, 0), 2)
                pixmap = self.convert_cv_to_pixmap(rect_frame)
                if pixmap:
                    self.video_widget.setPixmap(pixmap.scaled(
                        self.video_widget.width(),
                        self.video_widget.height(),
                        Qt.KeepAspectRatio))

    def mouseReleaseEvent(self, event):
        """Finalize the ROI rectangle."""
        if event.button() == Qt.LeftButton and event.y() < self.video_widget.height():
            x_offset = event.pos().x() - self.video_widget.x()
            y_offset = event.pos().y() - self.video_widget.y()
            if x_offset >= 0 and y_offset >= 0 and self.roi_start:
                x1, y1 = min(self.roi_start[0], x_offset), min(self.roi_start[1], y_offset)
                x2, y2 = max(self.roi_start[0], x_offset), max(self.roi_start[1], y_offset)
                width, height = x2 - x1, y2 - y1

                # Set ROI rectangle
                if width > 0 and height > 0:
                    self.roi_rect = QRect(x1, y1, width, height)

                    # Extract ROI from current frame and process it
                    roi_frame = np.copy(self.current_frame[y1:y2, x1:x2])
                    processed_frame = cv2.Canny(roi_frame, 50, 150)  # Edge detection

                    roi_pixmap = self.convert_cv_to_pixmap(roi_frame)
                    processed_pixmap = self.convert_cv_to_pixmap(processed_frame)

                    if roi_pixmap:
                        self.left_viewer.setPixmap(roi_pixmap.scaled(
                            self.left_viewer.width(), 
                            self.left_viewer.height(), 
                            Qt.KeepAspectRatio))

                    if processed_pixmap:
                        self.right_viewer.setPixmap(processed_pixmap.scaled(
                            self.right_viewer.width(), 
                            self.right_viewer.height(), 
                            Qt.KeepAspectRatio))

    def convert_cv_to_pixmap(self, cv_img):
        """Convert OpenCV image to QPixmap."""
        height, width = cv_img.shape[:2]
        
        if len(cv_img.shape) == 3:  # Color image (BGR to RGB)
            cv_img_rgb = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
            qimg = QImage(cv_img_rgb.data, width, height,
                          cv_img_rgb.strides[0], QImage.Format_RGB888)
        
        else:  # Grayscale image
            qimg = QImage(cv_img.data, width, height,
                          cv_img.strides[0], QImage.Format_Grayscale8)

        return QPixmap.fromImage(qimg)


# Main application loop
if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

### **추가된 기능**
1. **ROI 실시간 업데이트**:
   - `mouseMoveEvent`에서 드래그 중인 영역을 메인 뷰어에 실시간으로 그려줍니다.
   - OpenCV를 사용하여 현재 프레임에 사각형을 그린 후 업데이트합니다.

2. **ROI 유지**:
   - `mouseReleaseEvent`에서 설정된 ROI를 저장하고 메인 뷰어에 계속 표시합니다.

3. **ROI 처리 및 출력**:
   - 왼쪽 하단 뷰어에 선택한 ROI를 표시합니다.
   - 오른쪽 하단 뷰어에 해당 ROI의 엣지 검출 결과를 표시합니다.

---

## **실행 방법**

1. 코드를 Python 파일로 저장합니다(예: `pyqt5_roi_video_player.py`).
2. 필요한 라이브러리를 설치합니다:

   ```bash
   pip install pyqt5 opencv-python-headless numpy
   ```

3. 프로그램을 실행하고 동영상을 선택한 후 마우스로 드래그하여 ROI를 설정하면 하단 뷰어에 결과가 표시됩니다.

출처

-------------    
# 수정 버젼.
```python
import sys
import cv2
import numpy as np
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QHBoxLayout,
    QPushButton, QWidget, QLabel, QFileDialog
)
from PyQt5.QtCore import QTimer, Qt, QRect
from PyQt5.QtGui import QPixmap, QImage


class VideoPlayer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 Video Player with ROI")
        self.setGeometry(100, 100, 1200, 800)

        # Video capture
        self.cap = None
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)

        # ROI variables
        self.roi_start = None
        self.roi_rect = None
        self.current_frame = None

        # UI setup
        self.create_ui()

    def create_ui(self):
        main_layout = QVBoxLayout()

        # Main video viewer
        self.video_widget = QLabel()
        self.video_widget.setStyleSheet("background-color: black;")
        main_layout.addWidget(self.video_widget)

        # Control buttons
        control_layout = QHBoxLayout()
        self.btn_open = QPushButton("Open Video")
        self.btn_open.clicked.connect(self.open_file)

        self.btn_play_pause = QPushButton("Play")
        self.btn_play_pause.clicked.connect(self.play_pause_video)

        control_layout.addWidget(self.btn_open)
        control_layout.addWidget(self.btn_play_pause)
        main_layout.addLayout(control_layout)

        # ROI and processed viewers
        viewers_layout = QHBoxLayout()

        self.roi_viewer = QLabel("ROI Viewer")
        self.roi_viewer.setFixedSize(400, 300)
        self.roi_viewer.setStyleSheet("background-color: black;")

        self.processed_viewer = QLabel("Processed Viewer")
        self.processed_viewer.setFixedSize(400, 300)
        self.processed_viewer.setStyleSheet("background-color: black;")

        viewers_layout.addWidget(self.roi_viewer)
        viewers_layout.addWidget(self.processed_viewer)
        main_layout.addLayout(viewers_layout)

        central_widget = QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

    def open_file(self):
        file_name, _ = QFileDialog.getOpenFileName(self, "Open Video", "", "Video Files (*.avi *.mp4 *.mkv)")
        if file_name:
            self.cap = cv2.VideoCapture(file_name)
            if not self.cap.isOpened():
                print("Error: Cannot open video.")
                return
            self.timer.start(30)

    def play_pause_video(self):
        if self.cap and self.cap.isOpened():
            if self.timer.isActive():
                self.timer.stop()
                self.btn_play_pause.setText("Play")
            else:
                self.timer.start(30)
                self.btn_play_pause.setText("Pause")

    def update_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            self.timer.stop()
            return

        self.current_frame = frame.copy()

        # Draw ROI on the frame
        if self.roi_rect:
            x1, y1, x2, y2 = self.roi_rect.getCoords()
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

            # Update ROI viewer and processed viewer
            roi_frame = self.current_frame[y1:y2, x1:x2]
            processed_frame = cv2.Canny(roi_frame, 50, 150) if roi_frame.size > 0 else None

            if roi_frame.size > 0:
                roi_pixmap = self.convert_cv_to_pixmap(roi_frame)
                if roi_pixmap:
                    self.roi_viewer.setPixmap(roi_pixmap.scaled(self.roi_viewer.width(), self.roi_viewer.height(), Qt.KeepAspectRatio))

            if processed_frame is not None:
                processed_pixmap = self.convert_cv_to_pixmap(processed_frame)
                if processed_pixmap:
                    self.processed_viewer.setPixmap(processed_pixmap.scaled(self.processed_viewer.width(), self.processed_viewer.height(), Qt.KeepAspectRatio))

        # Convert frame to QPixmap and display in the main viewer
        pixmap = self.convert_cv_to_pixmap(frame)
        if pixmap:
            self.video_widget.setPixmap(pixmap.scaled(self.video_widget.width(), self.video_widget.height(), Qt.KeepAspectRatio))

    def mousePressEvent(self, event):
        """Start drawing ROI when the mouse is pressed."""
        if event.button() == Qt.LeftButton and self.video_widget.underMouse():
            self.roi_start = (event.pos().x(), event.pos().y())

    def mouseMoveEvent(self, event):
        """Draw ROI rectangle dynamically as the mouse moves."""
        if event.buttons() == Qt.LeftButton and self.roi_start:
            x1, y1 = self.roi_start
            x2, y2 = event.pos().x(), event.pos().y()
            self.roi_rect = QRect(x1, y1, x2 - x1, y2 - y1)

    def mouseReleaseEvent(self, event):
        """Finalize the ROI when the mouse is released."""
        if event.button() == Qt.LeftButton and self.roi_start:
            x1, y1 = self.roi_start
            x2, y2 = event.pos().x(), event.pos().y()
            self.roi_rect = QRect(x1, y1, x2 - x1, y2 - y1)
            self.roi_start = None

    def convert_cv_to_pixmap(self, cv_img):
        """Convert OpenCV image to QPixmap."""
        height, width = cv_img.shape[:2]
        if len(cv_img.shape) == 3:  # Color image (BGR to RGB)
            cv_img = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
            qimg = QImage(cv_img.data, width, height, cv_img.strides[0], QImage.Format_RGB888)
        else:  # Grayscale image
            qimg = QImage(cv_img.data, width, height, cv_img.strides[0], QImage.Format_Grayscale8)
        return QPixmap.fromImage(qimg)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    player = VideoPlayer()
    player.show()
    sys.exit(app.exec_())
```




