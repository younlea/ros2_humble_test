다음은 녹화 및 정지 버튼을 추가하고, 영상을 저장하는 기능을 기존 코드에 통합할 수 있도록 UI 부분과 녹화 저장 기능만 작성한 예제입니다.

1. UI 코드 수정 (init_ui)

녹화/정지 버튼 추가와 토글 동작 구현:

self.record_button = QPushButton("Start Recording")
self.record_button.setCheckable(True)
self.record_button.clicked.connect(self.toggle_recording)
button_layout.addWidget(self.record_button)

2. 녹화 기능 추가

클래스 변수와 관련 함수 정의:

클래스 변수 추가

self.is_recording = False  # 녹화 상태
self.video_writer = None   # VideoWriter 객체

녹화 토글 동작 함수

def toggle_recording(self):
    if self.is_recording:
        # 녹화 중지
        self.is_recording = False
        self.record_button.setText("Start Recording")
        
        # 녹화 종료
        if self.video_writer is not None:
            self.video_writer.release()
            self.video_writer = None
            print("Recording stopped.")
    else:
        # 녹화 시작
        self.is_recording = True
        self.record_button.setText("Stop Recording")
        
        # 비디오 파일 저장 준비
        current_time = time.strftime("%Y%m%d_%H%M%S")
        filename = f"capture_image/{current_time}.avi"
        
        frame_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        frame_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        fps = int(self.cap.get(cv2.CAP_PROP_FPS)) or 30  # 기본값 30fps
        
        self.video_writer = cv2.VideoWriter(
            filename, 
            cv2.VideoWriter_fourcc(*'XVID'), 
            fps, 
            (frame_width, frame_height)
        )
        print(f"Recording started: {filename}")

3. 녹화 데이터 저장 처리

update_frame 함수 수정하여 녹화 상태일 때 frame을 저장:

def update_frame(self):
    ret, frame = self.cap.read()
    if ret:
        self.current_frame = frame
        display_frame = frame.copy()

        # 녹화 중이면 영상 저장
        if self.is_recording and self.video_writer is not None:
            self.video_writer.write(frame)

        # 스트리밍 프레임을 보여줌
        frame_rgb = cv2.cvtColor(display_frame, cv2.COLOR_BGR2RGB)
        h, w, ch = frame_rgb.shape
        bytes_per_line = ch * w
        qimg = QImage(frame_rgb.data, w, h, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(qimg)
        self.video_label.setPixmap(pixmap)

4. 결과 동작 요약
	•	UI
	•	“Start Recording” 버튼을 누르면 녹화가 시작됩니다.
	•	녹화 중 “Stop Recording” 버튼을 누르면 녹화가 중지됩니다.
	•	저장
	•	비디오 파일은 capture_image/ 디렉토리에 현재 시간 기반의 파일 이름으로 저장됩니다.
	•	코드 통합
	•	위 함수와 UI 코드를 기존 코드의 적절한 위치에 추가하면 됩니다.

추가 참고 사항
	1.	폴더 생성
capture_image 디렉토리가 없으면 녹화 시 에러가 발생할 수 있으니, 실행 전에 디렉토리를 미리 만들어주세요.
자동으로 생성하려면 다음 코드를 초기화 시점에 추가하세요:

import os
if not os.path.exists("capture_image"):
    os.makedirs("capture_image")


	2.	녹화 중인 영상 확인
영상 저장 포맷은 XVID로 설정되어 있으니, 대부분의 플레이어에서 재생이 가능합니다.
필요에 따라 cv2.VideoWriter_fourcc를 변경해 다른 포맷으로 저장할 수도 있습니다.

위 코드를 추가하면 녹화 기능이 기존 UI와 통합됩니다. 문제가 있거나 더 필요한 기능이 있다면 알려주세요!
