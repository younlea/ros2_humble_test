아래는 Python을 사용하여 AVI 파일을 열고, 영상을 재생하며, 영상 컨트롤 버튼 (+10초, +5초, 정지/시작, -5초, -10초)을 추가하는 프로그램 코드입니다.    
이 코드는 `tkinter`를 사용하여 파일 선택 팝업창과 버튼 UI를 구현하고, `OpenCV`를 사용하여 영상을 재생합니다.

## **전체 코드**

```python
import cv2
import tkinter as tk
from tkinter import filedialog
from tkinter import ttk

class VideoPlayer:
    def __init__(self, root):
        self.root = root
        self.root.title("AVI Video Player")
        
        # Video variables
        self.video_path = None
        self.cap = None
        self.is_playing = False
        self.current_frame = 0
        
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
            
            # Display the frame on the canvas
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame_image = cv2.imencode('.ppm', frame_rgb)[1].tobytes()
            
            photo = tk.PhotoImage(data=frame_image)
            self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            
            # Update the current frame index and refresh the UI
            self.current_frame += 1
            
            # Allow tkinter to refresh the UI properly
```

출처
