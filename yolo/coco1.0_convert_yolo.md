COCO 1.0으로 데이터를 export한 경우, 해당 데이터셋은 COCO 형식의 JSON 파일과 이미지로 구성되어 있습니다. 이를 YOLO에서 학습하려면 COCO 형식의 주석 파일(JSON)을 YOLO 형식으로 변환해야 합니다. 아래는 COCO 1.0 데이터를 YOLO 형식으로 변환하고 학습하는 방법을 단계별로 설명합니다.

---

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

이 과정을 따르면 CVAT에서 export한 COCO 데이터를 성공적으로 YOLO 형식으로 변환하고 학습할 수 있습니다!

출처
