COCO 데이터셋을 YOLO에서 학습 데이터로 사용하려면 COCO 형식의 주석 파일(JSON)을 YOLO 형식의 텍스트 파일로 변환해야 합니다. 아래는 COCO 데이터를 YOLO 형식으로 변환하고, YOLOv5 또는 YOLOv8에서 학습하는 방법을 단계별로 설명합니다.

---

## **1. COCO 데이터를 YOLO 형식으로 변환**

### **방법 1: Ultralytics의 `convert_coco` 함수 사용**
Ultralytics에서 제공하는 Python 도구를 사용하여 COCO 형식을 YOLO로 변환할 수 있습니다.
1. Python 환경에서 Ultralytics 라이브러리를 설치합니다:
   ```bash
   pip install ultralytics
   ```
2. `convert_coco` 함수를 사용하여 변환:
   ```python
   from ultralytics.data.converter import convert_coco

   # COCO JSON 파일 경로와 출력 디렉토리 지정
   convert_coco(
       labels_dir="path/to/coco/annotations/",  # COCO 주석 파일 경로
       save_dir="path/to/yolo/annotations/",    # YOLO 형식으로 저장할 경로
       use_segments=False,                      # 세그멘테이션 포함 여부 (False)
       use_keypoints=False                      # 키포인트 포함 여부 (False)
   )
   ```
3. 변환이 완료되면 YOLO 형식의 `.txt` 파일이 생성됩니다.

### **방법 2: GitHub 오픈소스 도구 사용**
- [COCO-json-annotations-to-YOLO-txt-format-converter](https://github.com/enekuie/COCO-json-annotations-to-YOLO-txt-format-converter) 같은 오픈소스 도구를 사용할 수 있습니다.
1. 저장소를 클론하고 의존성을 설치합니다:
   ```bash
   git clone https://github.com/enekuie/COCO-json-annotations-to-YOLO-txt-format-converter.git
   cd COCO-json-annotations-to-YOLO-txt-format-converter
   pip install -r requirements.txt
   ```
2. 스크립트를 실행하여 변환:
   ```bash
   python convert.py --input_path path/to/coco.json --output_path path/to/yolo/
   ```

### **방법 3: Roboflow 사용**
Roboflow 플랫폼을 통해 COCO 데이터를 업로드하고 YOLO 형식으로 변환할 수 있습니다.
1. Roboflow에 가입하고 새 프로젝트를 생성합니다.
2. COCO JSON 파일과 이미지를 업로드합니다.
3. "Export Dataset" 옵션에서 **YOLO Darknet TXT** 또는 **YOLOv8 PyTorch TXT** 형식을 선택하여 다운로드합니다.

---

## **2. 데이터셋 구성**

변환된 데이터셋은 다음과 같은 구조를 가져야 합니다:
```
dataset/
  ├── images/
  │   ├── train/      # 학습 이미지
  │   ├── val/        # 검증 이미지
  └── labels/
      ├── train/      # 학습 라벨 (YOLO 형식)
      ├── val/        # 검증 라벨 (YOLO 형식)
```

각 `.txt` 파일은 해당 이미지의 객체 정보를 포함하며, 다음과 같은 형식을 가집니다:
```
<class_id> <x_center> <y_center> <width> <height>
```
- 좌표는 이미지 크기에 대해 정규화된 값 ([0, 1] 범위)입니다.

---

## **3. `data.yaml` 파일 작성**

YOLO 모델이 데이터를 인식할 수 있도록 `data.yaml` 파일을 작성합니다:
```yaml
path: ./dataset  # 데이터셋 루트 폴더 경로
train: images/train  # 학습 이미지 경로
val: images/val      # 검증 이미지 경로

nc: <클래스 개수>     # 클래스 개수 (예: 80 for COCO)
names:               # 클래스 이름 리스트
  - person
  - bicycle
  - car
  ...
```

---

## **4. YOLO 모델 학습**

### **YOLOv5 학습**
1. YOLOv5 저장소를 클론하고 의존성을 설치합니다:
   ```bash
   git clone https://github.com/ultralytics/yolov5.git
   cd yolov5
   pip install -r requirements.txt
   ```
2. 학습 명령어 실행:
   ```bash
   python train.py --img 640 --batch 16 --epochs 50 --data data.yaml --weights yolov5s.pt --name coco_model
   ```

### **YOLOv8 학습**
1. Ultralytics 패키지를 설치합니다:
   ```bash
   pip install ultralytics
   ```
2. CLI 명령어로 학습 실행:
   ```bash
   yolo task=detect mode=train model=yolov8n.pt data=data.yaml epochs=50 imgsz=640 batch=16 name=coco_model
   ```
3. Python API를 사용하여 학습:
   ```python
   from ultralytics import YOLO

   model = YOLO("yolov8n.pt")  # 사전 학습된 모델 로드
   model.train(data="data.yaml", epochs=50, imgsz=640, batch=16)
   ```

---

## **5. 결과 확인 및 테스트**

### **검증**
- 학습된 모델의 성능을 검증합니다.
- YOLOv5:
  ```bash
  python val.py --weights runs/train/coco_model/weights/best.pt --data data.yaml --img 640
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=val model=runs/detect/coco_model/weights/best.pt data=data.yaml imgsz=640
  ```

### **테스트**
- 새로운 이미지나 비디오에서 모델을 테스트합니다.
- YOLOv5:
  ```bash
  python detect.py --weights runs/train/coco_model/weights/best.pt --source path_to_test_images --conf 0.25
  ```
- YOLOv8:
  ```bash
  yolo task=detect mode=predict model=runs/detect/coco_model/weights/best.pt source=path_to_test_images conf=0.25
  ```

---

이 과정을 따르면 COCO 데이터셋을 성공적으로 변환하여 YOLOv5 또는 YOLOv8에서 사용할 수 있습니다!

출처
[1] How To Convert COCO JSON to YOLO Keras TXT - Roboflow https://roboflow.com/convert/coco-json-to-yolo-keras-txt
[2] COCO-json-annotations-to-YOLO-txt-format-converter - GitHub https://github.com/enekuie/COCO-json-annotations-to-YOLO-txt-format-converter
[3] Convert YOLO annotations to COCO/Pascal VOC - Kaggle https://www.kaggle.com/code/siddharthkumarsah/convert-yolo-annotations-to-coco-pascal-voc
[4] Taeyoung96/Yolo-to-COCO-format-converter - GitHub https://github.com/Taeyoung96/Yolo-to-COCO-format-converter
[5] How To Convert COCO JSON to YOLO Darknet TXT - Roboflow https://roboflow.com/convert/coco-json-to-yolo-darknet-txt
[6] converter - Ultralytics YOLO Docs https://docs.ultralytics.com/reference/data/converter/
[7] Object Detection Datasets Overview - Ultralytics YOLO Docs https://docs.ultralytics.com/datasets/detect/
[8] How To Convert COCO JSON to YOLOv8 PyTorch TXT - Roboflow https://roboflow.com/convert/coco-json-to-yolov8-pytorch-txt
[9] Convert YOLO and COCO Annotations to DagsHub Format https://dagshub.com/blog/convert-annotations-to-dagshub/
[10] COCO json annotation to YOLO txt format [closed] - Stack Overflow https://stackoverflow.com/questions/68398965/coco-json-annotation-to-yolo-txt-format/71830725
[11] ultralytics/JSON2YOLO: Convert JSON annotations into YOLO format. https://github.com/ultralytics/JSON2YOLO
[12] [Dataset] COCO dataset to YOLO - velog https://velog.io/@cjh2626002/Dataset-COCO-dataset-to-YOLO
[13] How to convert segmentation annotations from coco to yolo format? https://stackoverflow.com/questions/74629147/how-to-convert-segmentation-annotations-from-coco-to-yolo-format
[14] 02). Convert2Yolo 소개 · GitBook https://deepbaksuvision.github.io/Modu_ObjectDetection/posts/02_02_Convert2Yolo.html
[15] Train - Ultralytics YOLO Docs https://docs.ultralytics.com/modes/train/
[16] YOLOv8: A Complete Guide [2025 Update] - viso.ai https://viso.ai/deep-learning/yolov8-guide/
[17] YOLOv5: Expert Guide to Custom Object Detection Training https://learnopencv.com/custom-object-detection-training-using-yolov5/
[18] How to Train Ultralytics YOLOv8 models on Your Custom Dataset in ... https://www.youtube.com/watch?v=LNwODJXcvt4
[19] How to Train YOLOv8 Object Detection on a Custom Dataset https://blog.roboflow.com/how-to-train-yolov8-on-a-custom-dataset/
[20] Comprehensive Guide to Ultralytics YOLOv5 https://docs.ultralytics.com/yolov5/
[21] What is YOLOv8? A Complete Guide - Roboflow Blog https://blog.roboflow.com/what-is-yolov8/
[22] How to Train YOLO v5 on a Custom Dataset - DigitalOcean https://www.digitalocean.com/community/tutorials/train-yolov5-custom-data
[23] Tips for Best Training Results - Ultralytics YOLO Docs https://docs.ultralytics.com/yolov5/tutorials/tips_for_best_training_results/
[24] train-yolov8-object-detection-on-custom-dataset.ipynb - GitHub https://github.com/roboflow/notebooks/blob/main/notebooks/train-yolov8-object-detection-on-custom-dataset.ipynb
[25] tutorial.ipynb - ultralytics/yolov5 - GitHub https://github.com/ultralytics/yolov5/blob/master/tutorial.ipynb?short_path=3f7133f
[26] How to Train a YOLOv11 Object Detection Model on a Custom Dataset https://blog.roboflow.com/yolov11-how-to-train-custom-data/
[27] Object Detection Datasets Overview - Ultralytics YOLO Docs https://docs.ultralytics.com/datasets/detect/
[28] How to Train a YOLOv5 Model On a Custom Dataset - Roboflow Blog https://medium.com/towards-data-science/how-to-train-a-custom-object-detection-model-with-yolo-v5-917e9ce13208
[29] Train YOLOv8 on Custom Dataset – A Complete Tutorial https://learnopencv.com/train-yolov8-on-custom-dataset/
[30] Train Custom Data - Ultralytics YOLO Docs https://docs.ultralytics.com/yolov5/tutorials/train_custom_data/
[31] Train Custom Data · ultralytics/yolov5 Wiki - GitHub https://github.com/ultralytics/yolov5/wiki/Train-Custom-Data/aaeb8cd6ab3cce202f46ceb8ce61e191bc9f5b2b
[32] Preparing Data for YOLO Training: Data Annotation Techniques and ... https://www.basic.ai/blog-post/data-annotation-for-yolo-model-training-techniques-and-best-practices-1
[33] Training the YOLOv5 Object Detector on a Custom Dataset https://pyimagesearch.com/2022/06/20/training-the-yolov5-object-detector-on-a-custom-dataset/
[34] How to Train a YOLO Model with Locally Downloaded Open Images ... https://stackoverflow.com/questions/77855248/how-to-train-a-yolo-model-with-locally-downloaded-open-images-dataset/77861789
