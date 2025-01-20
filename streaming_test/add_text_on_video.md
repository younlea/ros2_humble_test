아래는 OpenCV를 사용하여 **메인 뷰어의 오른쪽 상단에 현재 시간과 원하는 텍스트를 두 줄로 표시**하는 코드입니다. `cv2.putText`를 활용하여 각 프레임에 텍스트를 추가합니다.

---

## **코드**

```python
import cv2
import datetime

def add_text_to_frame(frame, text, position, font=cv2.FONT_HERSHEY_SIMPLEX, font_scale=1, color=(0, 255, 0), thickness=2):
    """
    Adds text to the given frame at the specified position.
    """
    cv2.putText(frame, text, position, font, font_scale, color, thickness, cv2.LINE_AA)

# Open a video file or capture from webcam
cap = cv2.VideoCapture(0)  # Use 0 for webcam or replace with video file path

if not cap.isOpened():
    print("Error: Cannot open video source.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("End of video or cannot read frame.")
        break

    # Get current date and time
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    custom_text = "Your Custom Text Here"

    # Define positions for the text
    frame_height, frame_width = frame.shape[:2]
    time_position = (frame_width - 400, 50)  # Adjust x and y for top-right corner
    text_position = (frame_width - 400, 100)  # Below the time

    # Add date/time and custom text to the frame
    add_text_to_frame(frame, current_time, time_position)
    add_text_to_frame(frame, custom_text, text_position)

    # Display the frame
    cv2.imshow("Video with Text", frame)

    # Exit on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

---

## **코드 설명**

### **1. `add_text_to_frame` 함수**
- 텍스트를 프레임에 추가하는 함수입니다. OpenCV의 `cv2.putText`를 사용합니다.
- 매개변수:
  - `frame`: 텍스트를 추가할 프레임.
  - `text`: 표시할 텍스트 문자열.
  - `position`: 텍스트의 왼쪽 하단 좌표 (x, y).
  - `font`: 텍스트의 글꼴 (기본값: `cv2.FONT_HERSHEY_SIMPLEX`).
  - `font_scale`: 글꼴 크기 배율 (기본값: `1`).
  - `color`: 텍스트 색상 (기본값: 초록색 `(0, 255, 0)`).
  - `thickness`: 텍스트 두께 (기본값: `2`).

### **2. 현재 시간과 사용자 정의 텍스트**
- 현재 시간을 Python의 `datetime` 모듈을 사용하여 가져옵니다:
  ```python
  current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
  ```
- 사용자 정의 텍스트는 간단히 문자열로 설정합니다:
  ```python
  custom_text = "Your Custom Text Here"
  ```

### **3. 위치 조정**
- 오른쪽 상단에 텍스트를 표시하기 위해 프레임의 너비와 높이를 기준으로 위치를 계산합니다:
  ```python
  frame_height, frame_width = frame.shape[:2]
  time_position = (frame_width - 400, 50)   # 시간 텍스트 위치
  text_position = (frame_width - 400, 100) # 사용자 정의 텍스트 위치
  ```

### **4. 반복문에서 프레임 업데이트**
- 각 프레임에 시간을 업데이트하고 사용자 정의 텍스트를 추가합니다.
- OpenCV의 `cv2.imshow`로 결과를 표시합니다.

---

## **결과**

1. **오른쪽 상단에 두 줄의 텍스트**:
   - 첫 번째 줄: 현재 시간 (예: "2025-01-20 15:00:00").
   - 두 번째 줄: 사용자 정의 텍스트 (예: "Your Custom Text Here").

2. 프로그램 실행 중 실시간으로 시간이 업데이트됩니다.

3. 'q' 키를 누르면 프로그램이 종료됩니다.

---

## **사용 방법**

1. 위 코드를 복사하여 Python 파일로 저장합니다(예: `video_with_text.py`).
2. OpenCV가 설치되어 있지 않다면 설치합니다:
   ```bash
   pip install opencv-python
   ```
3. 프로그램을 실행하면 웹캠 또는 비디오 파일에서 오른쪽 상단에 시간이 표시된 영상을 볼 수 있습니다.

이 코드는 간단히 확장 가능하며 원하는 위치나 스타일로 텍스트를 조정할 수 있습니다!

출처
[1] Display date and time in videos using OpenCV Python - TutorialsPoint https://www.tutorialspoint.com/display-date-and-time-in-videos-using-opencv-python
[2] Show Date and Time on Videos using OpenCV Python - GitHub Gist https://gist.github.com/pknowledge/9eb4c1302e0932629c25ce9924999c18
[3] Python OpenCv Write text on video - YouTube https://www.youtube.com/watch?v=Un2XZwh9eos
[4] Show Date and Time on Videos using OpenCV Python - YouTube https://www.youtube.com/watch?v=rRSyg9kYfcU
[5] 3. Video에 frame 및 text 표시하기 - yongyong-e - 티스토리 https://yongyong-e.tistory.com/45
[6] How to add text to video in Opencv Python tutorial - YouTube https://www.youtube.com/watch?v=SnLdUHLbBqU
[7] Display date and time in videos using OpenCV - Python https://www.geeksforgeeks.org/display-date-and-time-in-videos-using-python-opencv/
[8] How to display text on video in Python OpenCv - YouTube https://www.youtube.com/watch?v=JB-LyaqoJpU
[9] How can I put text on video using opencv in python? - Sololearn https://www.sololearn.com/en/Discuss/1508273/how-can-i-put-text-on-video-using-opencv-in-python
[10] Python OpenCV | cv2.putText() method - GeeksforGeeks https://www.geeksforgeeks.org/python-opencv-cv2-puttext-method/
[11] Python OpenCv: Write text on video - GeeksforGeeks https://www.geeksforgeeks.org/python-opencv-write-text-on-video/
[12] how to print a text to a frame in opencv-python - Stack Overflow https://stackoverflow.com/questions/25931692/how-to-print-a-text-to-a-frame-in-opencv-python
[13] Extract a frame at a specific time of a video and insert a text OpenCV ... https://stackoverflow.com/questions/59007475/extract-a-frame-at-a-specific-time-of-a-video-and-insert-a-text-opencv-python
[14] 컴퓨터 비전 cv2.putText 활용 하여 label과 score 글씨 출력하기. https://swmakerjun.tistory.com/48
[15] Add text over live webcam video in Opencv Python tutorial - YouTube https://www.youtube.com/watch?v=h__hDejVjPs
[16] i want to display time in my video using opencv. edit https://answers.opencv.org/question/9955/i-want-to-display-time-in-my-video-using-opencv/
[17] Rendering Responsive Text on Video using Python - Sonsuz Design https://sonsuzdesign.blog/2021/04/06/rendering-responsive-text-on-video-using-python/
[18] OpenCV: printing live time countdown on a video display https://forums.raspberrypi.com/viewtopic.php?t=233412
