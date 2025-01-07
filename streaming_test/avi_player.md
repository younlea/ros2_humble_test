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

출처
