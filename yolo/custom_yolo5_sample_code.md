YOLOv5 모델을 OpenCV와 함께 사용하여 들어온 이미지에서 객체를 탐지하는 방법을 단계별로 설명합니다. 아래 코드는 PyTorch와 OpenCV를 사용하여 YOLOv5 모델을 로드하고, 이미지를 입력받아 객체 탐지 결과를 시각화하는 데 중점을 둡니다.

---

## **1. 필요한 라이브러리 설치**
YOLOv5와 OpenCV를 사용하려면 다음 라이브러리를 설치해야 합니다:
```bash
pip install torch torchvision opencv-python ultralytics
```

---

## **2. 코드 작성**

### **2.1 YOLOv5 모델 로드**
학습한 YOLOv5 모델(`best.pt`)을 로드합니다. PyTorch Hub를 사용하면 간단하게 모델을 불러올 수 있습니다.

### **2.2 OpenCV로 이미지 읽기 및 전처리**
YOLOv5는 RGB 형식의 이미지를 입력으로 받으므로, OpenCV에서 읽은 이미지를 BGR에서 RGB로 변환해야 합니다.

### **2.3 YOLOv5 추론 및 결과 시각화**
YOLOv5의 결과를 받아 바운딩 박스와 클래스 이름을 원본 이미지에 그립니다.

---

### **Python 코드 예제**
```python
import cv2
import torch

# 1. YOLOv5 모델 로드
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')  # 학습한 모델 경로 지정

# 2. OpenCV로 이미지 읽기
image_path = 'input.jpg'  # 테스트할 이미지 경로
img = cv2.imread(image_path)

# 3. BGR -> RGB 변환 (YOLOv5는 RGB 형식 필요)
rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

# 4. YOLOv5 추론 실행
results = model(rgb_img)

# 5. 결과 출력 (바운딩 박스, 클래스 이름)
results.print()  # 탐지된 객체 정보 출력

# 6. 결과를 원본 이미지에 그리기
results.render()  # 바운딩 박스와 클래스 이름을 이미지에 추가
output_img = results.imgs[0]  # 결과 이미지 가져오기 (numpy 배열 형태)

# 7. OpenCV로 출력 이미지 표시
cv2.imshow('YOLOv5 Detection', output_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
```

---

## **3. 주요 코드 설명**

### **3.1 모델 로드**
```python
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')
```
- `path='best.pt'`: 학습한 YOLOv5 모델의 경로입니다.
- PyTorch Hub를 사용하면 간단하게 커스텀 모델을 불러올 수 있습니다.

### **3.2 추론 실행**
```python
results = model(rgb_img)
```
- `results`: 탐지된 객체의 클래스, 바운딩 박스 좌표, 신뢰도 등의 정보를 포함합니다.

### **3.3 결과 시각화**
```python
results.render()
output_img = results.imgs[0]
```
- `results.render()`: 탐지 결과(바운딩 박스 및 클래스 이름)를 원본 이미지에 그립니다.
- `results.imgs`: 처리된 이미지를 numpy 배열 형태로 반환합니다.

---

## **4. 실시간 웹캠 객체 탐지**

웹캠에서 실시간으로 객체를 탐지하려면 다음 코드를 사용하세요:
```python
import cv2
import torch

# YOLOv5 모델 로드
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')

# 웹캠 열기
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # BGR -> RGB 변환
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # YOLOv5 추론 실행
    results = model(rgb_frame)

    # 결과 그리기
    results.render()
    output_frame = results.imgs[0]

    # OpenCV로 출력 프레임 표시
    cv2.imshow('YOLOv5 Webcam Detection', output_frame)

    # 'q' 키를 누르면 종료
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

---

## **5. 추가 팁**

### **결과 데이터 처리**
`results` 객체에는 탐지된 객체의 상세 정보가 포함되어 있습니다:
```python
for *box, conf, cls in results.xyxy[0]:  # xyxy 형식으로 바운딩 박스 좌표 가져오기
    print(f"Class: {int(cls)}, Confidence: {conf}, Box: {box}")
```
- `box`: 바운딩 박스 좌표 `[x_min, y_min, x_max, y_max]`.
- `conf`: 신뢰도 점수.
- `cls`: 클래스 ID.

### **이미지 저장**
탐지 결과를 저장하려면 다음 코드를 추가하세요:
```python
cv2.imwrite('output.jpg', output_img)
```

---

위 코드를 사용하면 학습한 YOLOv5 모델을 OpenCV와 함께 활용하여 이미지 또는 실시간 스트림에서 객체 탐지를 수행할 수 있습니다! 🚀

출처
[1] Detecting objects with YOLOv5, OpenCV, Python and C++ https://pub.towardsai.net/detecting-objects-with-yolov5-opencv-python-and-c-c7cf13d1483c?gi=5ba2603c20d5
[2] Yolov5 with OpenCV - python - Stack Overflow https://stackoverflow.com/questions/74648336/yolov5-with-opencv
[3] 라즈베리파이5를 이용해 Opencv활용 방법 및 Yolov5를 이용한 ... https://discuss.pytorch.kr/t/5-opencv-yolov5/5557
[4] YOLOv5 Quickstart - Ultralytics YOLO https://docs.ultralytics.com/yolov5/quickstart_tutorial/
[5] How To Live YOLOv5 Model for Object Detection with OpenCV https://www.youtube.com/watch?v=JB2rCCJnu94
[6] Real-time Object Detection with YOLOv5 and OpenCV - YouTube https://www.youtube.com/watch?v=KQKwXga_uTM
[7] Can yolov5 use multi-images inference? · Issue #7526 - GitHub https://github.com/ultralytics/yolov5/issues/7526
[8] Implement YOLOV5 With OpenCV From Scratch In Python - YouTube https://www.youtube.com/watch?v=B5ganPjMOAY
[9] How can I use image as input for inference? · Issue #7044 - GitHub https://github.com/ultralytics/yolov5/issues/7044
[10] OpenCV를 이용하여 video object detection하기(with Yolov5) https://stupidly-honest.tistory.com/3
[11] [opencv] YOLOv5 .pt 파일 .onnx 변환 & C++에서 추론하기 - velog https://velog.io/@nooleongyee/YOLOv5-.pt-%ED%8C%8C%EC%9D%BC-.onnx-%EB%B3%80%ED%99%98-C%EC%97%90%EC%84%9C-%EC%B6%94%EB%A1%A0%ED%95%98%EA%B8%B0
[12] PyTorch: OpenCV와 YOLOv5 모델을 이용한 동영상 객체 탐지 https://foss4g.tistory.com/1647
[13] Yolov5 image classification in C++ - OpenCV Forum https://forum.opencv.org/t/yolov5-image-classification-in-c/11578
[14] learnopencv/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in ... https://github.com/spmallick/learnopencv/blob/master/Object-Detection-using-YOLOv5-and-OpenCV-DNN-in-CPP-and-Python/yolov5.cpp
[15] 라즈베리파이 내 yolov5 동작의 문제점 - 묻고 답하기 https://discuss.pytorch.kr/t/yolov5/4526
[16] poojatambe/Yolov5-inference-on-ONNXRuntime-and-opencv-DNN https://github.com/poojatambe/Yolov5-inference-on-ONNXRuntime-and-opencv-DNN
[17] Object Detection in 10 minutes with YOLOv5 & Python! - YouTube https://www.youtube.com/watch?v=fu2tfOV9vbY
[18] Object Detection Inference in Python with YOLOv5 and PyTorch https://stackabuse.com/object-detection-inference-in-python-with-yolov5-and-pytorch/
[19] Yolov5 with OpenCV - python - Stack Overflow https://stackoverflow.com/questions/74648336/yolov5-with-opencv
[20] [python] yolov5이용, 간단한 객체 학습 후 탐지 https://yobbicorgi.tistory.com/34
[21] Object Detection using YOLOv5 OpenCV DNN in C++ and Python https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
[22] Implement YOLOV5 With OpenCV From Scratch In Python - YouTube https://www.youtube.com/watch?v=B5ganPjMOAY
[23] doleron/yolov5-opencv-cpp-python - GitHub https://github.com/doleron/yolov5-opencv-cpp-python
