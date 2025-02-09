아래는 학습된 YOLOv5 모델(`best.pt`)을 사용하여 단일 이미지를 로드하고, 객체를 탐지한 후 바운딩 박스를 그려 결과를 시각화하는 Python 코드입니다.

---

## **Python 코드: YOLOv5로 이미지 탐지 및 박싱**

```python
import cv2
import torch

# 1. YOLOv5 모델 로드
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')  # 학습된 모델 경로 지정

# 2. 이미지 로드
image_path = 'input.jpg'  # 테스트할 이미지 경로
img = cv2.imread(image_path)

# 3. BGR -> RGB 변환 (YOLOv5는 RGB 형식 필요)
rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

# 4. YOLOv5 추론 실행
results = model(rgb_img)

# 5. 탐지 결과 가져오기
detections = results.xyxy[0]  # xyxy 포맷으로 탐지 결과 가져오기

# 6. 탐지된 객체를 원본 이미지에 박싱 및 라벨링
for detection in detections:
    x_min, y_min, x_max, y_max, conf, cls = detection.tolist()
    class_id = int(cls)  # 클래스 ID
    confidence = float(conf)  # 신뢰도 점수

    # 바운딩 박스 그리기
    cv2.rectangle(img, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)

    # 클래스 이름과 신뢰도 표시
    label = f"{model.names[class_id]} {confidence:.2f}"
    cv2.putText(img, label, (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

# 7. 결과 이미지 출력
cv2.imshow('YOLOv5 Detection', img)
cv2.waitKey(0)
cv2.destroyAllWindows()

# 8. 결과 이미지 저장 (선택 사항)
cv2.imwrite('output.jpg', img)
```

---

## **코드 설명**

### **1. 모델 로드**
```python
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')
```
- `path='best.pt'`: 학습한 YOLOv5 모델의 가중치 파일 경로를 지정합니다.
- `torch.hub.load`: YOLOv5 모델을 PyTorch Hub를 통해 로드합니다.
- `custom`: 사용자 정의 모델을 로드하기 위한 옵션입니다.

### **2. 이미지 읽기**
```python
img = cv2.imread(image_path)
rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
```
- OpenCV로 이미지를 읽으면 기본적으로 BGR 형식으로 로드됩니다.
- YOLOv5는 RGB 형식을 입력으로 사용하므로 변환이 필요합니다.

### **3. 추론 실행**
```python
results = model(rgb_img)
detections = results.xyxy[0]
```
- `results.xyxy`: 탐지된 객체의 바운딩 박스 좌표와 클래스 정보를 포함합니다.
- 각 탐지 결과는 `[x_min, y_min, x_max, y_max, confidence, class_id]` 형식입니다.

### **4. 바운딩 박스 그리기**
```python
cv2.rectangle(img, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)
```
- OpenCV의 `rectangle` 함수를 사용하여 바운딩 박스를 그립니다.
- `(0, 255, 0)`는 초록색을 의미하며, 두께는 `2`입니다.

### **5. 클래스 이름과 신뢰도 표시**
```python
label = f"{model.names[class_id]} {confidence:.2f}"
cv2.putText(img, label, (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
```
- `model.names`: 학습된 모델에 저장된 클래스 이름 리스트입니다.
- 클래스 ID를 기반으로 이름과 신뢰도를 표시합니다.

### **6. 결과 시각화 및 저장**
```python
cv2.imshow('YOLOv5 Detection', img)
cv2.imwrite('output.jpg', img)
```
- OpenCV의 `imshow` 함수로 결과 이미지를 화면에 표시합니다.
- `imwrite` 함수로 결과 이미지를 파일에 저장할 수 있습니다.

---

## **결과 예시**
1. 입력 이미지(`input.jpg`)에서 학습한 모델이 객체를 탐지합니다.
2. 각 객체에 대해 바운딩 박스와 클래스 이름이 표시됩니다.
3. 결과 이미지는 화면에 표시되며(`imshow`), 필요하면 파일(`output.jpg`)로 저장됩니다.

---

## **추가 팁**

### **탐지 결과 데이터 확인**
탐지된 객체 정보를 출력하려면 다음 코드를 추가하세요:
```python
for detection in detections:
    x_min, y_min, x_max, y_max, conf, cls = detection.tolist()
    print(f"Class ID: {int(cls)}, Confidence: {conf:.2f}, Box: ({x_min}, {y_min}, {x_max}, {y_max})")
```

### **신뢰도 임계값 설정**
특정 신뢰도 이상의 객체만 표시하려면 다음 조건을 추가하세요:
```python
if confidence > 0.5:  # 신뢰도가 50% 이상인 경우만 처리
    # 바운딩 박스 및 라벨링 코드 삽입
```

---

위 코드를 사용하면 학습한 YOLOv5 모델을 사용하여 단일 이미지에서 객체를 탐지하고 시각화할 수 있습니다! 🚀

출처
