아래는 두 프로그램의 파이썬 코드입니다. 첫 번째는 RTSP 스트림을 송출하는 프로그램이고, 두 번째는 송출된 RTSP 스트림을 수신하고 표시하는 프로그램입니다. RTSP 스트리밍을 구현하기 위해 OpenCV와 GStreamer를 사용합니다.

프로그램 1: RTSP 스트림 송출기

이 프로그램은 이미지 두 장을 번갈아 가며 30fps로 출력하고, RTSP 프로토콜을 통해 송출합니다.

import cv2
import numpy as np
import time

# 이미지 파일 경로 설정
image1_path = "image1.jpg"  # 첫 번째 이미지 경로
image2_path = "image2.jpg"  # 두 번째 이미지 경로

# 이미지 로드
image1 = cv2.imread(image1_path)
image2 = cv2.imread(image2_path)

# 스트림 해상도 (이미지 크기와 일치하도록 설정)
frame_width = image1.shape[1]
frame_height = image1.shape[0]

# GStreamer 파이프라인 설정
rtsp_server = "rtsp://0.0.0.0:8554/test"
gst_pipeline = (
    f"appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! "
    f"rtph264pay config-interval=1 pt=96 ! udpsink host=127.0.0.1 port=8554"
)
out = cv2.VideoWriter(gst_pipeline, cv2.CAP_GSTREAMER, 0, 30, (frame_width, frame_height), True)

if not out.isOpened():
    print("Error: Could not open video stream.")
    exit()

print(f"RTSP Stream ready at {rtsp_server}")

# 송출 루프
try:
    while True:
        start_time = time.time()
        for _ in range(30):  # 30fps를 구현
            frame = image1 if int(time.time()) % 2 == 0 else image2
            out.write(frame)
            time.sleep(1 / 30.0)
        # 1초 대기
        time.sleep(max(1.0 - (time.time() - start_time), 0))
except KeyboardInterrupt:
    print("Streaming stopped.")
finally:
    out.release()

프로그램 2: RTSP 스트림 뷰어

이 프로그램은 첫 번째 프로그램에서 송출한 RTSP 스트림을 받아 화면에 표시합니다.

import cv2

# RTSP 스트림 주소
rtsp_stream = "rtsp://<송출기의 IP>:8554/test"

# GStreamer 파이프라인 설정
gst_pipeline = (
    f"rtspsrc location={rtsp_stream} latency=0 ! "
    f"rtph264depay ! avdec_h264 ! videoconvert ! appsink"
)

cap = cv2.VideoCapture(gst_pipeline, cv2.CAP_GSTREAMER)

if not cap.isOpened():
    print("Error: Could not open RTSP stream.")
    exit()

print(f"Connected to RTSP stream: {rtsp_stream}")

# 스트림 수신 및 표시 루프
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Frame not received.")
            break
        cv2.imshow("RTSP Stream Viewer", frame)
        if cv2.waitKey(1) & 0xFF == ord("q"):  # 'q'를 눌러 종료
            break
except KeyboardInterrupt:
    print("Viewer stopped.")
finally:
    cap.release()
    cv2.destroyAllWindows()

사용 방법
	1.	필요한 라이브러리 설치:

pip install opencv-python-headless

그리고 GStreamer가 설치되어 있어야 합니다. GStreamer 설치 방법은 OS에 따라 다르니, 설치를 진행하세요.

	2.	송출기 실행:
첫 번째 프로그램을 실행하여 RTSP 스트림을 송출합니다.
	3.	뷰어 실행:
두 번째 프로그램을 다른 IP에서 실행하고 <송출기의 IP>를 송출기의 실제 IP로 변경합니다.
	4.	종료 방법:
송출기는 Ctrl+C, 뷰어는 ‘q’ 키로 종료할 수 있습니다.

이미지 두 장(image1.jpg, image2.jpg)은 동일한 디렉터리에 준비해 두어야 합니다.
