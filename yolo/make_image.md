아래는 PyQt5와 OpenCV를 사용하여 AVI 파일을 열고, ROI(Region of Interest)를 선택한 뒤, 지정된 초당 캡처 빈도로 해당 영역을 캡처하여 JPG 파일로 저장하는 프로그램 코드입니다. 이 프로그램은 사용자가 ROI를 마우스로 선택하고, "Start Capturing" 버튼을 눌러 캡처를 시작하거나 중지할 수 있는 GUI를 제공합니다.

```python
import sys
import os
import cv2
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QFileDialog, QLabel, QSpinBox, QVBoxLayout, QWidget
)
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QPixmap, QImage

class VideoCaptureApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Video ROI Capture")
        self.setGeometry(100, 100, 800, 600)

        # 초기 변수 설정
        self.video_path = None
        self.cap = None
        self.timer = QTimer()
        self.roi = None
        self.capturing = False
        self.frame_interval = 1  # 초당 캡처 빈도

        # UI 구성 요소 생성
        self.init_ui()

    def init_ui(self):
        layout = QVBoxLayout()

        # 비디오 화면 표시용 라벨
        self.video_label = QLabel("Load a video to start")
        self.video_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(self.video_label)

        # 파일 열기 버튼
        self.open_button = QPushButton("Open Video")
        self.open_button.clicked.connect(self.open_video)
        layout.addWidget(self.open_button)

        # 초당 캡처 빈도 설정 스핀박스
        self.capture_rate_label = QLabel("Frames per second:")
        layout.addWidget(self.capture_rate_label)
        
        self.capture_rate_spinbox = QSpinBox()
        self.capture_rate_spinbox.setRange(1, 30)
        self.capture_rate_spinbox.setValue(1)
        layout.addWidget(self.capture_rate_spinbox)

        # ROI 설정 안내 라벨
        self.roi_label = QLabel("Select ROI by dragging on the video.")
        layout.addWidget(self.roi_label)

        # 캡처 시작/중지 버튼
        self.start_button = QPushButton("Start Capturing")
        self.start_button.setCheckable(True)
        self.start_button.clicked.connect(self.toggle_capture)
        layout.addWidget(self.start_button)

        # 메인 위젯 설정
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def open_video(self):
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getOpenFileName(self, "Open Video File", "", "Video Files (*.avi *.mp4)")
        
        if file_path:
            self.video_path = file_path
            self.cap = cv2.VideoCapture(file_path)
            if not os.path.exists("captured"):
                os.makedirs("captured")
            self.show_frame()

    def show_frame(self):
        if not self.cap or not self.cap.isOpened():
            return
        
        ret, frame = self.cap.read()
        
        if ret:
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            h, w, ch = frame_rgb.shape
            bytes_per_line = ch * w
            q_image = QImage(frame_rgb.data, w, h, bytes_per_line, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(q_image)
            self.video_label.setPixmap(pixmap.scaled(
                self.video_label.size(), Qt.KeepAspectRatio))
            
            # ROI 설정용 마우스 이벤트 연결
            if not hasattr(self.video_label, 'mousePressEvent'):
                def mousePressEvent(event):
                    if event.button() == Qt.LeftButton:
                        x1, y1 = event.pos().x(), event.pos().y()
                        def mouseReleaseEvent(event):
                            x2, y2 = event.pos().x(), event.pos().y()
                            x_min, x_max = sorted([x1, x2])
                            y_min, y_max = sorted([y1, y2])
                            self.roi = (x_min, y_min, x_max - x_min, y_max - y_min)
                            print(f"ROI selected: {self.roi}")
                            delattr(self.video_label, 'mouseReleaseEvent')
                        setattr(self.video_label.__class__, 'mouseReleaseEvent', mouseReleaseEvent)
                setattr(self.video_label.__class__, 'mousePressEvent', mousePressEvent)

    def toggle_capture(self):
        if not self.cap or not self.cap.isOpened():
            return
        
        if not self.capturing:
            if not self.roi:
                print("Please select an ROI before starting capture.")
                return
            
            # 캡처 시작 설정
            fps = int(self.cap.get(cv2.CAP_PROP_FPS))
            interval_ms = int(1000 / fps * (fps / max(1, int(fps / self.capture_rate_spinbox.value()))))
            
            print(f"Starting capture every {interval_ms} ms.")
            
            # 타이머 시작
            self.timer.timeout.connect(self.capture_frame)
            self.timer.start(interval_ms)
            
            # 버튼 상태 변경
            self.start_button.setText("Stop Capturing")
            self.capturing = True
        
        else:
            # 타이머 정지 및 상태 초기화
            print("Stopping capture.")
            
            self.timer.stop()
            
            # 버튼 상태 변경
            self.start_button.setText("Start Capturing")
            self.capturing = False

    def capture_frame(self):
        if not (self.cap and self.cap.isOpened() and self.roi):
            return
        
        ret, frame = self.cap.read()
        
        if ret:
            x, y, w, h = map(int, self.roi)
            
            roi_frame = frame[y:y+h, x:x+w]
            
            frame_count = int(self.cap.get(cv2.CAP_PROP_POS_FRAMES))
            
            save_path = os.path.join("captured", f"frame_{frame_count:04d}.jpg")
            
            cv2.imwrite(save_path, roi_frame)
            
            print(f"Captured and saved: {save_path}")
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = VideoCaptureApp()
    main_window.show()
    sys.exit(app.exec_())
```

### 주요 기능 설명:
1. **비디오 로드 및 표시**: `QFileDialog`를 사용하여 비디오 파일을 선택하고 OpenCV로 로드합니다. 첫 번째 프레임이 표시됩니다.
2. **ROI 선택**: 마우스 드래그 이벤트를 통해 ROI 영역을 선택합니다.
3. **캡처 시작/중지**: "Start Capturing" 버튼으로 캡처를 시작하거나 중지할 수 있습니다. 초당 몇 프레임을 캡처할지 `QSpinBox`로 설정합니다.
4. **프레임 저장**: OpenCV의 `cv2.imwrite`를 사용하여 선택한 ROI 영역을 JPG 파일로 저장합니다.

### 실행 방법:
1. 위 코드를 Python 스크립트로 저장합니다.
2. 필요한 라이브러리(OpenCV 및 PyQt5)를 설치합니다.
   ```bash
   pip install opencv-python-headless PyQt5
   ```
3. 스크립트를 실행하고 GUI에서 비디오 파일을 열어 테스트합니다.

출처
[1] how to read video data from memory use pyqt5 - Stack Overflow https://stackoverflow.com/questions/49215830/how-to-read-video-data-from-memory-use-pyqt5
[2] PyQt5 - open QMediaplayer in new window and play video https://stackoverflow.com/questions/53321617/pyqt5-open-qmediaplayer-in-new-window-and-play-video
[3] How can I play mp4 and avi files in a Movie Player? - Ask Ubuntu https://askubuntu.com/questions/30038/how-can-i-play-mp4-and-avi-files-in-a-movie-player
[4] ROIviewbox/ROI.py at master - GitHub https://github.com/mhogg/ROIviewbox/blob/master/ROI.py
[5] How to draw a rectangle and adjust its shape by drag and drop in ... https://stackoverflow.com/questions/44468775/how-to-draw-a-rectangle-and-adjust-its-shape-by-drag-and-drop-in-pyqt5
[6] Is there a selection widget? - Google Groups https://groups.google.com/g/pyqtgraph/c/do2Xq17m9Xo
[7] PyQt5 기반 동영상 플레이어앱 만들기 - 오션코딩학원 https://oceancoding.blogspot.com/2020/07/blog-post_22.html
[8] PyQt5 Video Player with QMediaPlayer - CodersLegacy https://coderslegacy.com/python/pyqt5-video-player-with-qmediaplayer/
[9] Thread: Video Avi and QT - Qt Centre Forum https://www.qtcentre.org/threads/48500-Video-Avi-and-QT
[10] QtMultimedia Media Player example can't play any video on Ubuntu ... https://forum.qt.io/topic/43468/qtmultimedia-media-player-example-can-t-play-any-video-on-ubuntu-14-04
[11] PyQt5 video widgets - Python https://pythonprogramminglanguage.com/pyqt5-video-widget/
[12] How to install PyQT 5 on UBUNTU ? | Qt Forum https://forum.qt.io/topic/44636/how-to-install-pyqt-5-on-ubuntu
[13] aviplay - QT-based movie player - Ubuntu Manpage https://manpages.ubuntu.com/manpages/bionic/man1/aviplay.1.html
[14] PyQt5 Tutorial #14 - Open Files with QFileDialog (2 Methods) https://www.youtube.com/watch?v=mFzNnIZSLHE
[15] Playing *.avi files with Qt - c++ - Stack Overflow https://stackoverflow.com/questions/38572619/playing-avi-files-with-qt
[16] Video Player with Qt5 - Python Forum https://python-forum.io/thread-1150.html
[17] How to display opencv video in pyqt apps - Discover gists · GitHub https://gist.github.com/docPhil99/ca4da12c9d6f29b9cea137b617c7b8b1
[18] WIth Qt,Video can play? - Stack Overflow https://stackoverflow.com/questions/25682858/with-qt-video-can-play?rq=3
[19] How do I set VLC media player as default video player? - Ask Ubuntu https://askubuntu.com/questions/91701/how-do-i-set-vlc-media-player-as-default-video-player
[20] PyQt5 open File Dialog and read file or ( browse File) - YouTube https://www.youtube.com/watch?v=uXlL2PeuLpQ
[21] Solved: PyQt Gui Application - STMicroelectronics Community https://community.st.com/t5/stm32-mpus-embedded-software-and/pyqt-gui-application/td-p/674670
[22] Using command line arguments to open files with PyQt5 apps https://forum.pythonguis.com/t/using-command-line-arguments-to-open-files-with-pyqt5-apps-windows-file-associations/232
[23] PyQt mousePressEvent - get object that was clicked on? https://stackoverflow.com/questions/27222016/pyqt-mousepressevent-get-object-that-was-clicked-on
[24] python - Selecting an area of an image with a mouse and recording ... https://stackoverflow.com/questions/55636313/selecting-an-area-of-an-image-with-a-mouse-and-recording-the-dimensions-of-the-s
[25] detect mouse button release event - Google Groups https://groups.google.com/g/pyqtgraph/c/INHpJWDlprs
[26] Save mouse-selected area | Qt Forum https://forum.qt.io/topic/134987/save-mouse-selected-area
[27] Mouse Event on Qlabel? - Qt Forum https://forum.qt.io/topic/83094/mouse-event-on-qlabel
[28] Draw rectangles using a mouse | PyQt5 Tutorial - Pinterest https://www.pinterest.com/pin/how-to-draw-rectangles-with-a-mouse--304767099794552429/
[29] PyQt5 mouse drag selection : 네이버 블로그 https://blog.naver.com/swkim4610/222098835116?viewType=pc
[30] How to add ROI with mouse down and drag interactively · Issue #2547 https://github.com/pyqtgraph/pyqtgraph/issues/2547
[31] Mark a rectangle when pressing and releasing left mouse button https://forum.qt.io/topic/137383/mark-a-rectangle-when-pressing-and-releasing-left-mouse-button
[32] Hello, I want to return to the item in the mouse click area ... - GitHub https://github.com/pyqtgraph/pyqtgraph/issues/2165
[33] Interactive Data Selection Controls - PyQtGraph - Read the Docs https://pyqtgraph.readthedocs.io/en/latest/user_guide/region_of_interest.html
[34] Use mouse drag to change the width of a rectangle? - Python GUIs https://www.pythonguis.com/faq/use-mouse-drag-to-change-the-width-of-a-rectangle/
[35] How to get frame from video by its index via OpenCV and Python? https://stackoverflow.com/questions/46100858/how-to-get-frame-from-video-by-its-index-via-opencv-and-python
[36] Extracting Frames from Video Using Python and OpenCV | Kaggle https://www.kaggle.com/general/491148
[37] Getting Started with Videos - OpenCV Documentation https://docs.opencv.org/4.x/dd/d43/tutorial_py_video_display.html
[38] How to capture a frame from real-time camera video using OpenCV https://www.educative.io/answers/how-to-capture-a-frame-from-real-time-camera-video-using-opencv
[39] Python - Extracting and Saving Video Frames - Stack Overflow https://stackoverflow.com/questions/33311153/python-extracting-and-saving-video-frames
[40] OpenCV. How do you catch a real time frame from OpenCV Video ... https://forum.opencv.org/t/opencv-how-do-you-catch-a-real-time-frame-from-opencv-video-capture/14880
[41] Extracting video frames using OpenCV - FutureLearn https://www.futurelearn.com/info/courses/introduction-to-image-analysis-for-plant-phenotyping/0/steps/305359
[42] How to Capture Frames from Camera with OpenCV in Python https://developer.ridgerun.com/wiki/index.php/How_to_Capture_Frames_from_Camera_with_OpenCV_in_Python
[43] OpenCV Python - Extract Images from Video - TutorialsPoint https://www.tutorialspoint.com/opencv_python/opencv_python_extract_images_video.htm
[44] How do I use Python PIL to save an image to a particular directory? https://stackoverflow.com/questions/31434278/how-do-i-use-python-pil-to-save-an-image-to-a-particular-directory
[45] #OpenCV – Open a video file and save each frame as a PNG file ... https://elbruno.com/2020/12/11/opencv-open-a-video-file-%F0%9F%8E%A5-and-save-each-frame-as-a-png-%F0%9F%96%BC-file-to-a-folder-%F0%9F%93%82python/
[46] 6 Ways to Save Images in Python - Cloudinary https://cloudinary.com/guides/web-performance/6-ways-to-save-images-in-python
[47] Function to extract frames from input video file using OpenCV and ... https://gist.github.com/arundasan91/25275b06d1a05f9f6ee34e572b58f756
[48] [OpenCV] 영상을 프레임별로 저장하기 (mp4 to jpg) - velog https://velog.io/@krec7748/%EC%98%81%EC%83%81%EC%9D%84-%ED%94%84%EB%A0%88%EC%9E%84%EB%B3%84%EB%A1%9C-%EC%A0%80%EC%9E%A5%ED%95%98%EA%B8%B0
[49] Tutorial 24 - Saving images from python to your local drive - YouTube https://www.youtube.com/watch?v=E_XZHQkQBBU
[50] Save an image in OpenCV without using "imwrite" function - Python https://forum.opencv.org/t/save-an-image-in-opencv-without-using-imwrite-function/4997
[51] Using PIL to save image to a relative directory : r/learnpython - Reddit https://www.reddit.com/r/learnpython/comments/4n17qo/using_pil_to_save_image_to_a_relative_directory/
[52] New to Python, help with saving image from camera https://python-forum.io/thread-20710.html
[53] Save Image to File in Python - Java2Blog https://java2blog.com/python-save-image-to-file/
[54] python - saving the images to a folder with custom filenames https://datascience.stackexchange.com/questions/39453/saving-the-images-to-a-folder-with-custom-filenames
[55] [파이썬 GUI 기초] PyQt5 - 7 토글버튼 / 체크박스 - 네이버 블로그 https://blog.naver.com/sagala_soske/221737055619
[56] 01-00. 버튼 기본설정 (QAbstractButton) - [Python 완전정복 시리즈] 3편 https://wikidocs.net/160772
[57] 01) QPushButton - 파이썬으로 만드는 나만의 GUI 프로그램 - 위키독스 https://wikidocs.net/21934
[58] QPushButton 토글(toggle) 기능 사용 - 예제로 배우는 PyQt https://opentutorials.org/module/544/18668
[59] Toggle switch Widget QCheckBox replacement - Python GUIs https://www.pythonguis.com/widgets/pyqt-toggle-widget/
[60] How to change push button text by clicking on another push button https://www.qtcentre.org/threads/70936-How-to-change-push-button-text-by-clicking-on-another-push-button
[61] Thread: QPushButton State (in PyQt) - Qt Centre Forum https://www.qtcentre.org/threads/61391-QPushButton-State-(in-PyQt)
[62] How to change button text in pyqt5 when clicked with keypress https://stackoverflow.com/questions/68807866/how-to-change-button-text-in-pyqt5-when-clicked-with-keypress
[63] how to create a toggle button(on/off button) - Qt Forum https://forum.qt.io/topic/96046/how-to-create-a-toggle-button-on-off-button
[64] PyQt5 Button issue - Qt Forum https://forum.qt.io/topic/113095/pyqt5-button-issue
[65] Scripts/PyQtexamples/pyqt_widgets_scripts/pyqt_toggle_button.py ... https://github.com/miura/Scripts/blob/master/PyQtexamples/pyqt_widgets_scripts/pyqt_toggle_button.py
[66] Button to +1 in text box everytime it's clicked (pyqt5) - Python Help https://discuss.python.org/t/button-to-1-in-text-box-everytime-its-clicked-pyqt5/52334
[67] PyQt5 VideoPlayer - GitHub Gist https://gist.github.com/Axel-Erfurt/af8bc3ff7dc11809b5ed3710af915b13
[68] ROI — pyqtgraph 0.14.0dev0 documentation - Read the Docs https://pyqtgraph.readthedocs.io/en/latest/api_reference/graphicsItems/roi.html
[69] Draw rectangles using a mouse | PyQt5 Tutorial - YouTube https://www.youtube.com/watch?v=3QRBk-FpWjE
[70] Drawing dots on top of image upon mouse clicks - Python GUIs Forum https://forum.pythonguis.com/t/drawing-dots-on-top-of-image-upon-mouse-clicks/201
[71] If mouse button event draw rectangle PyQt5 - Python Discussions https://discuss.python.org/t/if-mouse-button-event-draw-rectangle-pyqt5/6064
[72] Use mouse drag to change the width of a rectangle? - q&a https://forum.pythonguis.com/t/use-mouse-drag-to-change-the-width-of-a-rectangle/580
[73] How to Extract Frames from Video in Python https://thepythoncode.com/article/extract-frames-from-videos-in-python
[74] OpenCV Project - Extract Frames from a Video - DataFlair https://data-flair.training/blogs/opencv-extract-frames-from-video/
[75] Extracting Video Frames Using OpenCV in Python - YouTube https://www.youtube.com/watch?v=ECCHN1j_lis
[76] Extract frames from a video using Python and OpenCV - GitHub Gist https://gist.github.com/HaydenFaulkner/ef5fa6b990b385fbfb7edea544ff632b
[77] What is the fastest way to get all the frames from a video file? https://python-forum.io/thread-40045.html
[78] Extract Frame from Videos using OpenCV in Python - YouTube https://www.youtube.com/watch?v=SWGd2hX5p3U
[79] Save frames from video files as still images with OpenCV in Python https://note.nkmk.me/en/python-opencv-video-to-still-image/
[80] How To Save Images In Python? https://pythonguides.com/python-save-an-image-to-file/
[81] How to Save frames to directory - OpenCV Q&A Forum https://answers.opencv.org/question/21438/how-to-save-frames-to-directory/
[82] How To Save Frames From A Video In OpenCV Python - YouTube https://www.youtube.com/watch?v=hbJ_uAGxDY8
[83] How to save an image with Python - with code examples - Apify Blog https://blog.apify.com/save-image-python/
[84] how to write frames into folder using opencv - Stack Overflow https://stackoverflow.com/questions/71478115/how-to-write-frames-into-folder-using-opencv
[85] python - Save Video as Frames OpenCV{PY} - Stack Overflow https://stackoverflow.com/questions/27378662/save-video-as-frames-opencvpy
[86] [python] opencv video to image - 곰퓨타의 SW 이야기 - 티스토리 https://kom-story.tistory.com/161
[87] Create a toggle button with two states with PyQt5 [QPushButton] https://www.youtube.com/watch?v=wo9izoGm2Lw
[88] Ask for create a tutorial for Switch Button - Python GUIs Forum https://forum.pythonguis.com/t/ask-for-create-a-tutorial-for-switch-button/220
[89] How to change button text in PyQT5 when clicked with keypress? https://www.reddit.com/r/learnpython/comments/p5o519/how_to_change_button_text_in_pyqt5_when_clicked/
[90] Button which will be in pressed state when it's clicked - Qt Forum https://forum.qt.io/topic/89637/button-which-will-be-in-pressed-state-when-it-s-clicked
[91] PyQt5 update label on button click : r/learnpython - Reddit https://www.reddit.com/r/learnpython/comments/gkylp6/pyqt5_update_label_on_button_click/
[92] Programmatically Toggle a Python PyQt QPushbutton - Stack Overflow https://stackoverflow.com/questions/19508450/programmatically-toggle-a-python-pyqt-qpushbutton
[93] pyQt5. button 클릭 시 label text 변경 - ejyoo's 개발 노트 - 티스토리 https://ejyoo.tistory.com/197
[94] Python PyQt program - Button color changer - w3resource https://www.w3resource.com/python-exercises/pyqt/python-pyqt-widgets-exercise-5.php
