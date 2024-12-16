아래는 1920x1080 이미지를 320x240 비율에 맞춰 왜곡 없이 축소 및 패딩하여 표시하는 함수입니다. 이 함수는 OpenCV를 사용해 이미지를 320x240 크기로 조정하되, 비율을 유지하기 위해 이미지 주위에 검정색 패딩을 추가합니다.

resize_with_aspect_ratio 함수
```python
import cv2
import numpy as np

def resize_with_aspect_ratio(image, target_width, target_height):
    """
    이미지를 비율에 맞게 target_width x target_height 크기로 조정하고, 
    검정색 패딩을 추가하여 왜곡 없이 출력할 수 있도록 변환합니다.

    Args:
        image (numpy.ndarray): 원본 이미지
        target_width (int): 출력 이미지의 너비
        target_height (int): 출력 이미지의 높이

    Returns:
        numpy.ndarray: 크기가 조정되고 패딩된 이미지
    """
    # 원본 이미지 크기
    original_height, original_width = image.shape[:2]

    # 목표 비율 계산
    aspect_ratio_target = target_width / target_height
    aspect_ratio_original = original_width / original_height

    # 비율에 따라 새로운 크기 결정
    if aspect_ratio_original > aspect_ratio_target:
        # 가로가 더 긴 경우: 너비를 기준으로 조정
        new_width = target_width
        new_height = int(target_width / aspect_ratio_original)
    else:
        # 세로가 더 긴 경우: 높이를 기준으로 조정
        new_height = target_height
        new_width = int(target_height * aspect_ratio_original)

    # 이미지 리사이즈
    resized_image = cv2.resize(image, (new_width, new_height))

    # 패딩 계산
    top = (target_height - new_height) // 2
    bottom = target_height - new_height - top
    left = (target_width - new_width) // 2
    right = target_width - new_width - left

    # 검정색 패딩 추가
    padded_image = cv2.copyMakeBorder(resized_image, top, bottom, left, right, cv2.BORDER_CONSTANT, value=(0, 0, 0))

    return padded_image
```
함수 사용 방법

입력
	•	image: OpenCV로 읽어들인 이미지 (예: cv2.imread()로 로드한 numpy.ndarray).
	•	target_width, target_height: 출력 이미지 크기 (예: 320x240).

출력
	•	패딩된 이미지: 입력 이미지가 비율에 맞춰 조정되고 패딩이 포함된 결과 이미지.

적용 예제
```python
# 원본 이미지 로드 (예: 1920x1080 크기)
original_image = cv2.imread("example.jpg")

# 320x240 비율로 크기 조정 및 패딩
resized_image = resize_with_aspect_ratio(original_image, 320, 240)

# 결과 이미지 확인
cv2.imshow("Resized Image with Aspect Ratio", resized_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
```
캡처 이미지 출력 코드에 적용

update_captured_image 함수에 이 코드를 통합하면, 비율에 맞게 조정된 이미지를 QLabel에 출력할 수 있습니다.

```python
def update_captured_image(self, label, image):
    """지정된 QLabel에 비율 유지 및 패딩된 캡처 이미지를 표시"""
    image_resized = resize_with_aspect_ratio(image, 320, 240)  # 비율 유지 및 크기 조정
    image_rgb = cv2.cvtColor(image_resized, cv2.COLOR_BGR2RGB)  # OpenCV -> RGB 변환
    height, width, channel = image_rgb.shape
    bytes_per_line = 3 * width
    q_image = QImage(image_rgb.data, width, height, bytes_per_line, QImage.Format_RGB888)
    label.setPixmap(QPixmap.fromImage(q_image))
```
주요 특징
	•	왜곡 방지: 입력 이미지를 비율에 맞게 조정하므로 왜곡되지 않습니다.
	•	검정색 패딩: 출력 크기(320x240)에 맞춰 빈 공간은 검정색 패딩으로 채워집니다.
	•	다양한 크기 지원: target_width와 target_height 값을 조정해 다른 크기의 출력에도 사용 가능합니다.
