아래는 학습된 YOLOv5 모델(`best.pt`)을 사용하여 **`test.avi`** 비디오를 처리하고, 특정 클래스(예: `dish`)만 탐지하여 바운딩 박스를 그린 후 실시간으로 표시하는 Python 코드입니다.

---

## **Python 코드: 비디오에서 `dish` 탐지**

```python
import cv2
import torch

# 1. YOLOv5 모델 로드
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')  # 학습된 모델 경로 지정

# 2. 'dish' 클래스 ID 가져오기
# model.names는 클래스 이름 리스트를 포함합니다.
class_id_to_detect = None
for class_id, class_name in enumerate(model.names):
    if class_name == 'dish':  # 'dish' 클래스 이름 확인
        class_id_to_detect = class_id
        break

if class_id_to_detect is None:
    raise ValueError("Class 'dish' not found in the model's class names.")

# 3. 비디오 파일 열기
video_path = 'test.avi'
cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print(f"Error: Cannot open video {video_path}")
    exit()

# 4. 비디오 처리 및 객체 탐지
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # BGR -> RGB 변환 (YOLOv5는 RGB 형식 필요)
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # YOLOv5 추론 실행
    results = model(rgb_frame)
    detections = results.xyxy[0]  # xyxy 포맷으로 탐지 결과 가져오기

    # 탐지된 객체를 원본 프레임에 그리기
    for detection in detections:
        x_min, y_min, x_max, y_max, conf, cls = detection.tolist()
        if int(cls) == class_id_to_detect:  # 'dish' 클래스만 처리
            # 바운딩 박스 그리기
            cv2.rectangle(frame, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)

            # 클래스 이름과 신뢰도 표시
            label = f"dish {conf:.2f}"
            cv2.putText(frame, label, (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # 결과 프레임 표시
    cv2.imshow('YOLOv5 Dish Detection', frame)

    # 'q' 키를 누르면 종료
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

---

## **코드 설명**

### **1. YOLOv5 모델 로드**
```python
model = torch.hub.load('ultralytics/yolov5', 'custom', path='best.pt')
```
- `path='best.pt'`: 학습한 YOLOv5 모델의 가중치 파일 경로를 지정합니다.
- `torch.hub.load`: PyTorch Hub를 사용하여 YOLOv5 모델을 로드합니다.

---

### **2. 특정 클래스 ID 가져오기**
```python
class_id_to_detect = None
for class_id, class_name in enumerate(model.names):
    if class_name == 'dish':
        class_id_to_detect = class_id
        break
```
- `model.names`: 학습된 모델의 클래스 이름 리스트입니다.
- `'dish'` 클래스의 ID를 검색하여 저장합니다.
- 만약 `'dish'` 클래스가 없으면 에러를 발생시킵니다.

---

### **3. 비디오 파일 열기**
```python
cap = cv2.VideoCapture(video_path)
```
- OpenCV의 `VideoCapture` 객체를 사용하여 비디오 파일을 엽니다.
- `cap.read()`를 통해 프레임을 하나씩 읽어옵니다.

---

### **4. YOLOv5 추론 및 바운딩 박스 그리기**
```python
results = model(rgb_frame)
detections = results.xyxy[0]
```
- `results.xyxy`: 탐지된 객체의 바운딩 박스 좌표와 클래스 정보를 포함합니다.
- 각 탐지 결과는 `[x_min, y_min, x_max, y_max, confidence, class_id]` 형식입니다.

```python
if int(cls) == class_id_to_detect:
    cv2.rectangle(frame, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)
```
- 탐지된 객체 중 `'dish'` 클래스만 필터링하여 바운딩 박스를 그립니다.

---

### **5. 결과 표시**
```python
cv2.imshow('YOLOv5 Dish Detection', frame)
```
- OpenCV의 `imshow` 함수로 탐지 결과를 실시간으로 표시합니다.
- `'q'` 키를 누르면 프로그램이 종료됩니다.

---

## **6. 추가 기능**

### **6.1 결과 저장**
탐지 결과를 동영상 파일로 저장하려면 OpenCV의 `VideoWriter`를 사용할 수 있습니다:
```python
# 비디오 저장 설정 추가 (초기화 단계에서)
output_path = 'output.avi'
fourcc = cv2.VideoWriter_fourcc(*'XVID')
fps = int(cap.get(cv2.CAP_PROP_FPS))
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))

# 프레임 저장 추가 (반복문 안에서)
out.write(frame)

# 종료 시 리소스 해제 추가
out.release()
```

---

### **6.2 신뢰도 임계값 설정**
특정 신뢰도 이상의 객체만 표시하려면 다음 조건을 추가하세요:
```python
if int(cls) == class_id_to_detect and conf > 0.5:  # 신뢰도 > 50%
    # 바운딩 박스 및 라벨링 코드 삽입
```

---

위 코드를 사용하면 학습한 YOLOv5 모델로 비디오에서 특정 클래스(예: `dish`)만 탐지하고 바운딩 박스를 그릴 수 있습니다! 🚀

출처
