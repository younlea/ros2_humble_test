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

