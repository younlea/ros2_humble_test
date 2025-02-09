
[cv](http://10.113.174.173:8080/projects/11?page=1) 에서 DataSet export (coco 1.0 dataset 으로 다운로드)    



## **1. COCO 데이터셋 구조 확인**

CVAT에서 COCO 1.0으로 export한 데이터셋은 일반적으로 다음과 같은 구조를 가집니다:
```
dataset/
  ├── images/
  │   ├── train/      # 학습용 이미지
  │   ├── val/        # 검증용 이미지
  └── annotations/
      ├── instances_train.json  # 학습용 주석 파일
      ├── instances_val.json    # 검증용 주석 파일
```

- `images/train`와 `images/val`에는 이미지가 저장됩니다.
- `annotations/instances_train.json`와 `annotations/instances_val.json`에는 COCO 형식의 주석이 저장됩니다.

---

## **2. COCO 데이터를 YOLO 형식으로 변환**

YOLO는 `.txt` 파일로 바운딩 박스 정보를 저장하며, 각 이미지에 대해 하나의 `.txt` 파일이 생성됩니다. 이를 위해 COCO JSON 파일을 YOLO 형식으로 변환해야 합니다.

### **Python 스크립트: COCO to YOLO 변환**
다음 스크립트를 사용하여 COCO JSON 파일을 YOLO 형식으로 변환합니다:

```python
import os
import json
from tqdm import tqdm

def coco_to_yolo(coco_json_path, images_dir, output_dir):
    # COCO JSON 파일 읽기
    with open(coco_json_path, 'r') as f:
        coco_data = json.load(f)

    # 출력 디렉토리 생성
    os.makedirs(output_dir, exist_ok=True)

    # 이미지 정보와 클래스 정보 매핑
    images = {img['id']: img for img in coco_data['images']}
    categories = {cat['id']: cat['name'] for cat in coco_data['categories']}

    for ann in tqdm(coco_data['annotations']):
        img_id = ann['image_id']
        img_info = images[img_id]
        file_name = os.path.splitext(img_info['file_name'])[0]
        label_file = os.path.join(output_dir, f"{file_name}.txt")

        # 바운딩 박스 정보 가져오기
        bbox = ann['bbox']
        x_min, y_min, width, height = bbox
        x_center = (x_min + width / 2) / img_info['width']
        y_center = (y_min + height / 2) / img_info['height']
        width /= img_info['width']
        height /= img_info['height']

        class_id = ann['category_id']

        # YOLO 라벨 파일에 저장
        with open(label_file, 'a') as lf:
            lf.write(f"{class_id} {x_center} {y_center} {width} {height}\n")

# 실행 예시
coco_to_yolo(
    coco_json_path='dataset/annotations/instances_train.json',  # COCO 주석 파일 경로
    images_dir='dataset/images/train',                         # 이미지 디렉토리 경로
    output_dir='dataset/labels/train'                          # YOLO 라벨 저장 경로
)

coco_to_yolo(
    coco_json_path='dataset/annotations/instances_val.json',
    images_dir='dataset/images/val',
    output_dir='dataset/labels/val'
)
```

### **스크립트 설명**
- `coco_json_path`: COCO JSON 주석 파일 경로 (예: `instances_train.json`).
- `images_dir`: 이미지가 저장된 디렉토리 경로.
- `output_dir`: YOLO 라벨을 저장할 디렉토리 경로.
- 바운딩 박스 정보(`bbox`)를 읽어 YOLO 형식(`class_id x_center y_center width height`)으로 변환합니다.

---

## **3. 데이터셋 구조 정리**

변환 후 데이터셋은 다음과 같은 구조를 가져야 합니다:
```
dataset/
  ├── images/
  │   ├── train/      # 학습용 이미지
  │   ├── val/        # 검증용 이미지
  └── labels/
      ├── train/      # 학습용 라벨 (.txt)
      ├── val/        # 검증용 라벨 (.txt)
```

---

-----
### **Data set 분할 하기**

1. **Prepare the directory structure**:
   - Ensure you have the following directories:
     ```
     images/
     labels/
     ```
   - Create the output directories:
     ```
     mkdir -p images/train images/val labels/train labels/val
     ```

2. **Bash Script**:
   Use the following script to perform the split:

   ```bash
   #!/bin/bash

   # Paths to input and output directories
   IMAGES_DIR="images"
   LABELS_DIR="labels"
   TRAIN_IMAGES="images/train"
   VAL_IMAGES="images/val"
   TRAIN_LABELS="labels/train"
   VAL_LABELS="labels/val"

   # Create output directories if they don't exist
   mkdir -p "$TRAIN_IMAGES" "$VAL_IMAGES" "$TRAIN_LABELS" "$VAL_LABELS"

   # Get all file basenames (without extensions) from the images folder
   FILE_BASENAMES=$(find "$IMAGES_DIR" -type f -exec basename {} \; | sed 's/\.[^.]*$//' | sort | uniq)

   # Shuffle and split into 70% train and 30% val
   SHUFFLED_FILES=$(echo "$FILE_BASENAMES" | shuf)
   TOTAL_FILES=$(echo "$SHUFFLED_FILES" | wc -l)
   TRAIN_COUNT=$((TOTAL_FILES * 7 / 10))

   COUNTER=0
   for BASENAME in $SHUFFLED_FILES; do
       if [ $COUNTER -lt $TRAIN_COUNT ]; then
           mv "$IMAGES_DIR/$BASENAME".* "$TRAIN_IMAGES/" 2>/dev/null
           mv "$LABELS_DIR/$BASENAME".* "$TRAIN_LABELS/" 2>/dev/null
       else
           mv "$IMAGES_DIR/$BASENAME".* "$VAL_IMAGES/" 2>/dev/null
           mv "$LABELS_DIR/$BASENAME".* "$VAL_LABELS/" 2>/dev/null
       fi
       COUNTER=$((COUNTER + 1))
   done

   echo "Files successfully split into train and val sets."
   ```

3. **Explanation**:
   - The script identifies all unique file basenames in the `images` folder.
   - It shuffles the list of basenames to ensure randomness.
   - The total number of files is calculated, and 70% of them are assigned to the training set, while the remaining 30% go to the validation set.
   - Files are moved to their respective `train` or `val` subdirectories in both `images` and `labels`, ensuring that files with matching basenames are kept together.

4. **Run the script**:
   Save the script as `split_files.sh`, make it executable (`chmod +x split_files.sh`), and run it:
   ```bash
   ./split_files.sh
   ```
-----

## **4. `data.yaml` 파일 작성**

YOLO 모델에서 사용할 수 있도록 설정 파일(`data.yaml`)을 작성합니다:
```yaml
path: ./dataset  # 데이터셋 루트 경로
train: images/train  # 학습 이미지 경로
val: images/val      # 검증 이미지 경로

nc: <클래스 개수>     # 클래스 개수 (예: CVAT에서 정의한 클래스 수)
names:               # 클래스 이름 리스트 (CVAT에서 정의한 클래스 이름)
  - class_1_name
  - class_2_name
  ...
```

- `nc`: 클래스 개수 (예: CVAT에서 정의한 클래스 수).
- `names`: 클래스 이름 리스트는 CVAT에서 정의한 클래스 이름을 그대로 사용합니다.

---

## **5. YOLO 모델 학습**

### **YOLOv5 학습**
1. YOLOv5 저장소를 클론하고 의존성을 설치합니다:
   ```bash
   git clone https://github.com/ultralytics/yolov5.git
   cd yolov5
   pip install -r requirements.txt
   ```
2. 학습 명령어 실행:
   ```bash
   python train.py --img 640 --batch 16 --epochs 50 --data data.yaml --weights yolov5s.pt --name custom_model
   ```

### **YOLOv8 학습**
1. Ultralytics 패키지를 설치합니다:
   ```bash
   pip install ultralytics
   ```
2. CLI 명령어로 학습 실행:
   ```bash
   yolo task=detect mode=train model=yolov8n.pt data=data.yaml epochs=50 imgsz=640 batch=16 name=custom_model
   ```
3. Python API를 사용하여 학습:
   ```python
   from ultralytics import YOLO

   model = YOLO("yolov8n.pt")  # 사전 학습된 모델 로드
   model.train(data="data.yaml", epochs=50, imgsz=640, batch=16)
   ```

---

## **6. 결과 확인 및 테스트**

### **검증**
- 모델 성능을 검증합니다.
- YOLOv5:
  ```bash
  python val.py --weights runs/train/custom_model/weights/best.pt --data data.yaml --img 640
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=val model=runs/detect/custom_model/weights/best.pt data=data.yaml imgsz=640
  ```

### **테스트**
- 새로운 이미지나 비디오에서 모델을 테스트합니다.
- YOLOv5:
  ```bash
  python detect.py --weights runs/train/custom_model/weights/best.pt --source path_to_test_images --conf 0.25
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=predict model=runs/detect/custom_model/weights/best.pt source=path_to_test_images conf=0.25
  ```

---

# sample code

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
