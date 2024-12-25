RTSP 영상 스트리밍과 YOLO 또는 SAM 처리를 별도의 쓰레드로 분리하여 메인 쓰레드에서의 병목현상을 줄이는 구조로 코드 가이드를 작성하겠습니다.

코드 구조
	1.	VideoStreamThread: RTSP 영상을 읽고 프레임을 공유하는 쓰레드.
	2.	ProcessingThread: POI 데이터를 YOLO 또는 SAM으로 처리하고 결과를 공유하는 쓰레드.
	3.	MainThread: GUI를 관리하며, 두 쓰레드에서 데이터를 가져와 화면에 표시.

코드 예제

import cv2
import threading
import queue
import time
import numpy as np

class VideoStreamThread(threading.Thread):
    def __init__(self, rtsp_url, frame_queue):
        super(VideoStreamThread, self).__init__()
        self.rtsp_url = rtsp_url
        self.frame_queue = frame_queue
        self.cap = cv2.VideoCapture(rtsp_url)
        self.running = True

    def run(self):
        while self.running:
            ret, frame = self.cap.read()
            if ret:
                # 프레임을 큐에 넣기
                if not self.frame_queue.full():
                    self.frame_queue.put(frame)
            else:
                print("Failed to read frame from RTSP stream.")
            time.sleep(0.03)  # 약간의 대기 시간을 줘서 CPU 사용량을 조절

    def stop(self):
        self.running = False
        self.cap.release()


class ProcessingThread(threading.Thread):
    def __init__(self, frame_queue, result_queue, poi_rect):
        super(ProcessingThread, self).__init__()
        self.frame_queue = frame_queue
        self.result_queue = result_queue
        self.poi_rect = poi_rect
        self.running = True

    def run(self):
        while self.running:
            if not self.frame_queue.empty():
                frame = self.frame_queue.get()
                poi_frame = self.extract_poi(frame)
                
                # 여기서 YOLO 또는 SAM 처리
                result = self.process_poi(poi_frame)
                
                # 결과를 큐에 넣기
                if not self.result_queue.full():
                    self.result_queue.put(result)

    def extract_poi(self, frame):
        x, y, w, h = self.poi_rect
        return frame[y:y + h, x:x + w]

    def process_poi(self, poi_frame):
        # 예제: 간단히 흑백 변환 (YOLO/SAM 처리 대신)
        gray_frame = cv2.cvtColor(poi_frame, cv2.COLOR_BGR2GRAY)
        return gray_frame

    def stop(self):
        self.running = False


class MainApp:
    def __init__(self, rtsp_url, poi_rect):
        self.frame_queue = queue.Queue(maxsize=10)
        self.result_queue = queue.Queue(maxsize=10)

        self.video_thread = VideoStreamThread(rtsp_url, self.frame_queue)
        self.processing_thread = ProcessingThread(self.frame_queue, self.result_queue, poi_rect)

        self.running = True

    def start(self):
        self.video_thread.start()
        self.processing_thread.start()

        while self.running:
            # RTSP 스트림 화면 표시
            if not self.frame_queue.empty():
                frame = self.frame_queue.get()
                cv2.imshow("RTSP Stream", frame)

            # 처리 결과 표시
            if not self.result_queue.empty():
                result = self.result_queue.get()
                cv2.imshow("Processed Result", result)

            # 종료 조건
            if cv2.waitKey(1) & 0xFF == ord('q'):
                self.stop()
                break

    def stop(self):
        self.running = False
        self.video_thread.stop()
        self.processing_thread.stop()
        cv2.destroyAllWindows()


# RTSP URL 및 관심 영역(POI) 설정
RTSP_URL = "rtsp://your_camera_url"
POI_RECT = (100, 100, 300, 300)  # x, y, w, h

if __name__ == "__main__":
    app = MainApp(RTSP_URL, POI_RECT)
    app.start()

코드 설명
	1.	VideoStreamThread
	•	RTSP URL에서 프레임을 읽어 frame_queue에 넣습니다.
	•	cap.read() 실패 시 적절한 처리 및 대기 시간을 추가.
	2.	ProcessingThread
	•	frame_queue에서 프레임을 받아 POI를 추출.
	•	YOLO 또는 SAM을 사용해 해당 프레임을 처리하고 결과를 result_queue에 넣습니다.
	•	처리 부분은 process_poi 함수에 작성.
	3.	MainApp
	•	메인 쓰레드에서 두 개의 쓰레드를 실행.
	•	VideoStreamThread의 프레임을 화면에 표시.
	•	ProcessingThread의 결과를 화면에 표시.

추가 사항
	1.	YOLO 또는 SAM 모델 로드를 ProcessingThread의 초기화 메서드(__init__)에 추가하세요.
	2.	result_queue의 결과는 필요에 따라 POI 좌표로 매핑하여 화면에 출력할 수 있습니다.
	3.	쓰레드 종료 시 stop() 메서드로 자원을 정리합니다.

장점
	•	RTSP 스트림과 영상 처리를 별도의 쓰레드로 분리해 병목현상 제거.
	•	큐를 사용하여 안정적으로 데이터 교환 가능.

위 코드로 시스템 성능을 최적화하고 안정성을 높일 수 있습니다. 추가 요구사항이 있으면 말씀해주세요!
