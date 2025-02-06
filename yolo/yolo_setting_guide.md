YOLO 1.1 포맷으로 학습하는 방법에 대해 한국어로 자세히 설명드리겠습니다. YOLOv5와 YOLOv8 각각의 학습 방법과 기본 데이터셋 선택에 대해서도 안내합니다.

---

## **1. 데이터셋 준비**

### **CVAT에서 YOLO 1.1 포맷으로 데이터셋 내보내기**
- CVAT에서 데이터를 **YOLO 1.1 포맷**으로 내보냅니다.
- 내보낸 데이터셋은 다음과 같은 구조를 가집니다:
  ```
  dataset/
    ├── images/
    │   ├── train/   # 학습용 이미지
    │   ├── val/     # 검증용 이미지
    └── labels/
        ├── train/   # 학습용 라벨 (YOLO 형식)
        ├── val/     # 검증용 라벨 (YOLO 형식)
  ```
- `labels` 폴더에 있는 각 `.txt` 파일은 해당 이미지의 바운딩 박스 정보를 포함하며, 다음 형식으로 저장됩니다:
  ```
  <class_id> <x_center> <y_center> <width> <height>
  ```
  - `class_id`: 클래스 ID (예: 원형 접시 = 0, 사각형 접시 = 1)
  - `x_center`, `y_center`: 바운딩 박스 중심 좌표 (이미지 크기에 대해 정규화된 값, [0, 1] 범위)
  - `width`, `height`: 바운딩 박스의 너비와 높이 (정규화된 값)

### **`data.yaml` 파일 생성**
- YOLO 모델이 데이터셋을 인식할 수 있도록 설정 파일(`data.yaml`)을 작성합니다:
  ```yaml
  path: ./dataset  # 데이터셋 루트 폴더 경로
  train: images/train  # 학습 이미지 경로
  val: images/val      # 검증 이미지 경로

  nc: 2                # 클래스 개수 (예: 원형 접시와 사각형 접시)
  names: ['circular', 'rectangular']  # 클래스 이름
  ```

---

## **2. YOLO 환경 설정**

### **YOLOv5 설치**
1. YOLOv5 저장소를 클론하고 의존성을 설치합니다:
   ```bash
   git clone https://github.com/ultralytics/yolov5.git
   cd yolov5
   pip install -r requirements.txt
   ```
2. 설치가 완료되면 다음 명령어로 테스트합니다:
   ```bash
   python detect.py --source data/images --weights yolov5s.pt --conf 0.25
   ```

### **YOLOv8 설치**
1. Ultralytics 패키지를 설치합니다:
   ```bash
   pip install ultralytics
   ```
2. 설치가 완료되면 다음 명령어로 테스트합니다:
   ```bash
   yolo task=detect mode=predict model=yolov8n.pt source=data/images conf=0.25
   ```

---

## **3. 모델 학습**

### **YOLOv5 학습**
1. 사전 학습된 모델(예: `yolov5s.pt`)을 사용하여 학습을 시작합니다.
2. 학습 명령어:
   ```bash
   python train.py --img 640 --batch 16 --epochs 50 --data data.yaml --weights yolov5s.pt --name custom_model
   ```
3. 주요 옵션 설명:
   - `--img`: 입력 이미지 크기 (기본값: `640`)
   - `--batch`: 배치 크기 (한 번에 처리할 이미지 개수)
   - `--epochs`: 학습 반복 횟수
   - `--data`: `data.yaml` 파일 경로
   - `--weights`: 사전 학습된 가중치 파일 (빈 문자열 `''`로 설정하면 처음부터 학습)

### **YOLOv8 학습**
1. 사전 학습된 YOLOv8 모델(예: `yolov8n.pt`)을 사용하여 학습을 시작합니다.
2. CLI를 사용한 학습 명령어:
   ```bash
   yolo task=detect mode=train model=yolov8n.pt data=data.yaml epochs=50 imgsz=640 batch=16 name=custom_model
   ```
3. Python 코드로 학습 시작하기:
   ```python
   from ultralytics import YOLO

   model = YOLO("yolov8n.pt")  # 사전 학습된 모델 로드
   model.train(data="data.yaml", epochs=50, imgsz=640, batch=16)
   ```

---

## **4. 검증 및 테스트**

### **검증**
- 학습이 완료되면 모델 성능을 검증합니다.
- YOLOv5:
  ```bash
  python val.py --weights runs/train/custom_model/weights/best.pt --data data.yaml --img 640
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=val model=runs/detect/train/weights/best.pt data=data.yaml imgsz=640
  ```

### **테스트**
- 새로운 이미지나 비디오에서 모델을 테스트합니다.
- YOLOv5:
  ```bash
  python detect.py --weights runs/train/custom_model/weights/best.pt --source path_to_test_images --conf 0.25
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=predict model=runs/detect/train/weights/best.pt source=path_to_test_images conf=0.25
  ```

---

## **5. 베이스 모델 선택**

### **YOLOv5 베이스 모델**
- 선택 가능한 모델: `yolov5n`, `yolov5s`, `yolov5m`, `yolov5l`, `yolov5x`
- 추천: 
  - 빠른 실험과 테스트를 위해 **`yolov5s.pt`**를 사용하는 것을 권장합니다.

### **YOLOv8 베이스 모델**
- 선택 가능한 모델: `yolov8n`, `yolov8s`, `yolov8m`, `yolov8l`, `yolov8x`
- 추천:
  - 빠른 실험과 성능 확인을 위해 **`yolov8n.pt`**를 사용하는 것이 적합합니다.

---

이 과정을 따라하면 YOLOv5 또는 YOLOv8에서 YOLO 포맷 데이터를 사용하여 원하는 객체 탐지 모델을 성공적으로 학습시킬 수 있습니다!

출처
