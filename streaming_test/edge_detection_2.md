정교한 모델로 은색 바트를 감지하고 추적하려면 **YOLO (You Only Look Once)**나 OpenCV DNN 모듈을 활용할 수 있습니다. 두 접근법은 다음과 같은 특징이 있습니다:
	•	YOLO: 사전 학습된 객체 탐지 모델로, 빠르고 정확하게 특정 객체를 탐지합니다. 사용자 정의 데이터로 재학습이 가능합니다.
	•	OpenCV DNN: 다양한 DNN 모델을 로드하여 객체 탐지를 수행할 수 있으며, YOLO와 비슷하게 사용할 수 있습니다.

아래에 두 방법을 각각 구현하는 코드를 제공합니다.

1. YOLO로 바트 탐지

단계
	1.	모델 다운로드:
	•	사전 학습된 YOLO 가중치 파일 (yolov4.weights)와 설정 파일 (yolov4.cfg)을 다운로드합니다. YOLO 공식 사이트 또는 OpenCV 모델 Zoo에서 받습니다.
	2.	클래스 파일 다운로드:
	•	YOLO 모델이 학습된 클래스 이름이 저장된 coco.names 파일을 다운로드합니다.
	3.	코드 구현:

import cv2
import numpy as np

class YOLOObjectDetector:
    def __init__(self, config_path, weights_path, class_names_path):
        # YOLO 모델 로드
        self.net = cv2.dnn.readNetFromDarknet(config_path, weights_path)
        self.net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
        self.net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)

        # 클래스 이름 로드
        with open(class_names_path, 'r') as f:
            self.classes = f.read().strip().split('\n')

        # 모델 설정
        self.input_width = 416
        self.input_height = 416

    def detect_objects(self, frame):
        """
        객체 탐지를 수행하고 탐지된 객체의 정보를 반환.
        """
        blob = cv2.dnn.blobFromImage(frame, 1 / 255.0, (self.input_width, self.input_height), swapRB=True, crop=False)
        self.net.setInput(blob)

        # 네트워크 출력 레이어 이름 가져오기
        layer_names = self.net.getLayerNames()
        output_layer_names = [layer_names[i[0] - 1] for i in self.net.getUnconnectedOutLayers()]

        # 추론 실행
        detections = self.net.forward(output_layer_names)

        height, width = frame.shape[:2]
        boxes = []
        confidences = []
        class_ids = []

        for output in detections:
            for detection in output:
                scores = detection[5:]
                class_id = np.argmax(scores)
                confidence = scores[class_id]

                if confidence > 0.5:  # 탐지 신뢰도 임계값
                    box = detection[0:4] * np.array([width, height, width, height])
                    (center_x, center_y, box_width, box_height) = box.astype("int")

                    x = int(center_x - (box_width / 2))
                    y = int(center_y - (box_height / 2))

                    boxes.append([x, y, int(box_width), int(box_height)])
                    confidences.append(float(confidence))
                    class_ids.append(class_id)

        # Non-Maximum Suppression으로 중복 제거
        indices = cv2.dnn.NMSBoxes(boxes, confidences, 0.5, 0.4)
        results = []
        if len(indices) > 0:
            for i in indices.flatten():
                results.append((boxes[i], confidences[i], class_ids[i]))

        return results

# YOLO 모델 초기화
yolo = YOLOObjectDetector("yolov4.cfg", "yolov4.weights", "coco.names")

cap = cv2.VideoCapture("rtsp://192.168.0.4")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # POI 영역만 탐지
    poi_rect = (100, 100, 400, 300)
    x, y, w, h = poi_rect
    poi_frame = frame[y:y+h, x:x+w]

    # YOLO로 객체 탐지
    results = yolo.detect_objects(poi_frame)

    # 탐지 결과 표시
    for (box, confidence, class_id) in results:
        label = f"{yolo.classes[class_id]}: {confidence:.2f}"
        x, y, w, h = box
        cv2.rectangle(poi_frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        cv2.putText(poi_frame, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # 결과 표시
    cv2.imshow("POI Frame", poi_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

2. OpenCV DNN 모듈로 객체 탐지

OpenCV의 DNN 모듈은 YOLO와 유사하지만 더 일반적인 DNN 모델을 지원합니다. 아래는 SSD (Single Shot Detector) 모델을 사용하는 예제입니다.

단계
	1.	모델 다운로드:
	•	SSD 모델에서 MobileNet-SSD 사전 학습된 모델을 다운로드합니다.
	•	파일:
	•	MobileNetSSD_deploy.caffemodel (가중치 파일)
	•	MobileNetSSD_deploy.prototxt (구조 파일)
	2.	코드 구현:

import cv2
import numpy as np

class SSDObjectDetector:
    def __init__(self, model_path, config_path):
        # DNN 모델 로드
        self.net = cv2.dnn.readNetFromCaffe(config_path, model_path)

    def detect_objects(self, frame):
        """
        SSD로 객체 탐지를 수행합니다.
        """
        height, width = frame.shape[:2]
        blob = cv2.dnn.blobFromImage(frame, 0.007843, (300, 300), 127.5)
        self.net.setInput(blob)
        detections = self.net.forward()

        results = []
        for i in range(detections.shape[2]):
            confidence = detections[0, 0, i, 2]
            if confidence > 0.5:  # 신뢰도 임계값
                class_id = int(detections[0, 0, i, 1])
                box = detections[0, 0, i, 3:7] * np.array([width, height, width, height])
                (x, y, x2, y2) = box.astype("int")
                results.append(((x, y, x2 - x, y2 - y), confidence, class_id))

        return results

# SSD 모델 초기화
ssd = SSDObjectDetector("MobileNetSSD_deploy.caffemodel", "MobileNetSSD_deploy.prototxt")

cap = cv2.VideoCapture("rtsp://192.168.0.4")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # POI 영역 설정
    poi_rect = (100, 100, 400, 300)
    x, y, w, h = poi_rect
    poi_frame = frame[y:y+h, x:x+w]

    # SSD로 객체 탐지
    results = ssd.detect_objects(poi_frame)

    # 탐지 결과 표시
    for (box, confidence, class_id) in results:
        label = f"Class {class_id}: {confidence:.2f}"
        x, y, w, h = box
        cv2.rectangle(poi_frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
        cv2.putText(poi_frame, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # 결과 표시
    cv2.imshow("POI Frame", poi_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

요약

YOLO
	•	장점: 더 높은 정확도와 다양한 객체 탐지 가능.
	•	단점: 설정 및 가중치 다운로드가 필요.

OpenCV DNN
	•	장점: 더 가볍고 간단한 설정.
	•	단점: 특정 객체 탐지에서 YOLO보다 정확도가 떨어질 수 있음.

어느 방법이 더 적합한가?
	•	빠르게 기존 객체 탐지 (사전 학습된 클래스): OpenCV DNN.
	•	특정 객체(은색 바트)를 감지하려면 YOLO를 Fine-Tuning하거나 추가적으로 사용자 정의 모델
