우분투에서 동작하는 Python 프로그램을 만들어드리겠습니다. 이 프로그램은 이미지에서 색깔 있는 박스들을 감지하고, 해당 위치에 새로운 박스를 그려주며 좌표를 출력합니다.​​​​​​​​​​​​​​​​

```python
def analyze_image_colors(self, img):
        """이미지의 주요 색상을 분석하여 동적으로 색상 범위 생성"""
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 이미지를 작은 블록으로 나누어 색상 분석
        h, w = hsv.shape[:2]
        block_size = 20
        dominant_colors = []
        
        for y in range(0, h, block_size):
            for x in range(0, w, block_size):
                block = hsv[y:y+block_size, x:x+block_size]
                if block.size > 0:
                    # 블록의 평균 HSV 값 계산
                    mean_hsv = np.mean(block.reshape(-1, 3), axis=0)
                    h_val, s_val, v_val = mean_hsv
                    
                    # 채도와 명도가 일정 이상인 색상만 고려 (회색/검정/흰색 제외)
                    if s_val > 30 and v_val > 30:
                        dominant_colors.append((int(h_val), int(s_val), int(v_val)))
        
        # 유사한 색상들을 그룹화하여 색상 범위 생성
        dynamic_ranges = []
        processed_hues = set()
        
        for h, s, v in dominant_colors:
            if h not in processed_hues:
                # 현재 색상 주변의 색상 범위 생성
                h_range = 15  # Hue 범위
                s_min = max(30, s - 50)
                s_max = min(255, s + 50)
                v_min = max(30, v - 50)
                v_max = min(255, v + 50)
                
                # 빨간색의 경우 특별 처리 (0도 근처)
                if h < 10 or h > 170:
                    if h < 10:
                        dynamic_ranges.append([(max(0, h-h_range), s_min, v_min), (min(179, h+h_range), s_max, v_max)])
                    else:
                        dynamic_ranges.append([(max(0, h-h_range), s_min, v_min), (179, s_max, v_max)])
                        dynamic_ranges.append([(0, s_min, v_min), (10, s_max, v_max)])
                else:
                    dynamic_ranges.append([(max(0, h-h_range), s_min, v_min), (min(179, h+h_range), s_max, v_max)])
                
                # 처리된 색상 범위 마킹
                for i in range(max(0, h-h_range), min(180, h+h_range+1)):
                    processed_hues.add(i)
        
        return dynamic_ranges
    
    def detect_boxes_by_color(self, img):
        """색상 기반 박스 감지 (정적 + 동적 색상 범위)"""
        boxes = []
        
        # HSV 변환
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 정적 색상 범위 정의 (기본 색상들)
        static_color_ranges = [
            # 빨간색 (여러 범위)
            [(0, 50, 50), (10, 255, 255)],      # 밝은 빨강
            [(0, 30, 30), (15, 255, 255)],      # 연한 빨강
            [(160, 50, 50), (179, 255, 255)],   # 진한 빨강
            [(170, 30, 30), (179, 255, 255)],   # 어두운 빨강
            
            # 주황색 (여러 범위)
            [(5, 50, 50), (25, 255, 255)],      # 밝은 주황
            [(10, 30, 30), (30, 255, 255)],     # 연한 주황
            [(8, 100, 100), (20, 255, 255)],    # 진한 주황
            
            # 노란색 (여러 범위)
            [(15, 50, 50), (35, 255, 255)],     # 밝은 노랑
            [(20, 30, 30), (40, 255, 255)],     # 연한 노랑
            [(25, 100, 100), (35, 255, 255)],   # 진한 노랑
            
            # 초록색 (여러 범위)
            [(35, 50, 50), (85, 255, 255)],     # 전체 초록 범위
            [(40, 30, 30), (80, 255, 255)],     # 연한 초록
            [(45, 100, 100), (75, 255, 255)],   # 진한 초록
            [(50, 80, 80), (70, 255, 255)],     # 순수 초록
            
            # 청록색/민트색
            [(80, 50, 50), (100, 255, 255)],    # 청록색
            [(85, 30, 30), (95, 255, 255)],     # 연한 청록
            
            # 파란색 (여러 범위)
            [(90, 50, 50), (130, 255, 255)],    # 전체 파랑 범위
            [(100, 30, 30), (125, 255, 255)],   # 연한 파랑
            [(105, 100, 100), (125, 255, 255)], # 진한 파랑
            [(110, 80, 80), (120, 255, 255)],   # 순수 파랑
            
            # 보라색 (여러 범위)
            [(125, 50, 50), (160, 255, 255)],   # 전체 보라 범위
            [(130, 30, 30), (155, 255, 255)],   # 연한 보라
            [(135, 100, 100), (150, 255, 255)], # 진한 보라
            
            # 분홍색/마젠타
            [(140, 50, 50), (170, 255, 255)],   # 분홍색
            [(145, 30, 30), (165, 255, 255)],   # 연한 분홍
            [(150, 100, 100), (160, 255, 255)], # 진한 분홍
            
            # 갈색/베이지 (낮은 채도)
            [(10, 30, 30), (20, 100, 200)],     # 갈색 계열
            [(0, 20, 50), (30, 80, 200)],       # 베이지/황토색
            
            # 회색 계열 (매우 낮은 채도)
            [(0, 0, 50), (179, 30, 200)],       # 밝은 회색
            [(0, 0, 30), (179, 50, 150)],       # 어두운 회색
            
            # 검은색에 가까운 어두운 색상들
            [(0, 50, 20), (179, 255, 80)],      # 어두운 유채색
            [(0, 30, 10), (179, 255, 60)],      # 매우 어두운 색상
            
            # 흰색에 가까운 밝은 색상들
            [(0, 0, 200), (179, 30, 255)],      # 밝은 색상 (낮은 채도)
            [(0, 0, 220), (179, 20, 255)],      # 매우 밝은 색상
        ]
        
        # 동적 색상 범위 생성
        dynamic_color_ranges = self.analyze_image_colors(img)
        
        # 정적 + 동적 색상 범위 결합
        all_color_ranges = static_color_ranges + dynamic_color_ranges
```


```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램
Usage: python box_detector.py <image_path>
"""

import cv2
import numpy as np
import argparse
import sys
import os
from collections import defaultdict

class BoxDetector:
    def __init__(self):
        # 박스 감지를 위한 파라미터
        self.min_box_area = 100  # 최소 박스 크기
        self.max_box_area = 50000  # 최대 박스 크기
        self.aspect_ratio_range = (0.1, 10)  # 가로세로 비율 범위
        
    def preprocess_image(self, img):
        """이미지 전처리"""
        # 가우시안 블러로 노이즈 제거
        blurred = cv2.GaussianBlur(img, (3, 3), 0)
        return blurred
    
    def detect_boxes_by_edges(self, img):
        """엣지 기반 박스 감지"""
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # 엣지 감지
        edges = cv2.Canny(gray, 50, 150, apertureSize=3)
        
        # 모폴로지 연산으로 엣지 연결
        kernel = np.ones((3, 3), np.uint8)
        edges = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel)
        
        # 컨투어 찾기
        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        boxes = []
        for contour in contours:
            # 컨투어를 직사각형으로 근사화
            epsilon = 0.02 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            
            # 4개의 꼭짓점을 가진 도형 (사각형) 찾기
            if len(approx) >= 4:
                x, y, w, h = cv2.boundingRect(contour)
                area = w * h
                aspect_ratio = w / h if h > 0 else 0
                
                # 크기와 비율 조건 확인
                if (self.min_box_area <= area <= self.max_box_area and 
                    self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def detect_boxes_by_color(self, img):
        """색상 기반 박스 감지"""
        boxes = []
        
        # HSV 변환
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 다양한 색상 범위 정의 (HSV)
        color_ranges = [
            # 빨간색
            [(0, 120, 70), (10, 255, 255)],
            [(160, 120, 70), (179, 255, 255)],
            # 초록색
            [(40, 120, 70), (80, 255, 255)],
            # 파란색
            [(100, 120, 70), (130, 255, 255)],
            # 노란색
            [(15, 120, 70), (35, 255, 255)],
            # 보라색
            [(130, 120, 70), (160, 255, 255)],
            # 주황색
            [(5, 120, 70), (15, 255, 255)],
        ]
        
        for lower, upper in color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 노이즈 제거
            kernel = np.ones((5, 5), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                area = w * h
                aspect_ratio = w / h if h > 0 else 0
                
                if (self.min_box_area <= area <= self.max_box_area and 
                    self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def remove_duplicate_boxes(self, boxes, overlap_threshold=0.5):
        """중복되는 박스 제거"""
        if not boxes:
            return []
        
        # 면적 기준으로 정렬
        boxes = sorted(boxes, key=lambda b: b[2] * b[3], reverse=True)
        
        filtered_boxes = []
        for box in boxes:
            x1, y1, w1, h1 = box
            is_duplicate = False
            
            for existing_box in filtered_boxes:
                x2, y2, w2, h2 = existing_box
                
                # 겹치는 영역 계산
                overlap_x = max(0, min(x1 + w1, x2 + w2) - max(x1, x2))
                overlap_y = max(0, min(y1 + h1, y2 + h2) - max(y1, y2))
                overlap_area = overlap_x * overlap_y
                
                area1 = w1 * h1
                area2 = w2 * h2
                union_area = area1 + area2 - overlap_area
                
                if union_area > 0:
                    iou = overlap_area / union_area
                    if iou > overlap_threshold:
                        is_duplicate = True
                        break
            
            if not is_duplicate:
                filtered_boxes.append(box)
        
        return filtered_boxes
    
    def detect_boxes(self, img_path):
        """메인 박스 감지 함수"""
        # 이미지 로드
        img = cv2.imread(img_path)
        if img is None:
            raise ValueError(f"이미지를 로드할 수 없습니다: {img_path}")
        
        # 이미지 전처리
        processed_img = self.preprocess_image(img)
        
        # 다양한 방법으로 박스 감지
        edge_boxes = self.detect_boxes_by_edges(processed_img)
        color_boxes = self.detect_boxes_by_color(processed_img)
        
        # 모든 박스 합치기
        all_boxes = edge_boxes + color_boxes
        
        # 중복 제거
        final_boxes = self.remove_duplicate_boxes(all_boxes)
        
        return img, final_boxes
    
    def draw_boxes_and_save(self, img, boxes, output_path):
        """박스를 그리고 결과 이미지 저장"""
        result_img = img.copy()
        
        # 다양한 색상 정의 (BGR)
        colors = [
            (0, 255, 0),    # 초록
            (255, 0, 0),    # 파랑
            (0, 0, 255),    # 빨강
            (255, 255, 0),  # 청록
            (255, 0, 255),  # 마젠타
            (0, 255, 255),  # 노랑
            (128, 0, 128),  # 보라
            (255, 165, 0),  # 주황
        ]
        
        for i, (x, y, w, h) in enumerate(boxes):
            color = colors[i % len(colors)]
            
            # 박스 그리기 (굵기 2)
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            
            # 박스 번호 표시
            cv2.putText(result_img, f'{i+1}', (x, y-5), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
        
        # 결과 이미지 저장
        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다.')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 파일명 (기본값: output.jpg)')
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화
        detector = BoxDetector()
        
        print(f"이미지 분석 중: {args.image_path}")
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            return
        
        # 결과 출력
        print(f"\n감지된 박스 개수: {len(boxes)}")
        print("-" * 50)
        print("박스 좌표 (x, y, w, h):")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            print(f"박스 {i}: ({x}, {y}, {w}, {h})")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original', img)
            cv2.imshow('Detected Boxes', result_img)
            print("\n'q' 키를 눌러 종료하세요.")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        except cv2.error:
            print("GUI 환경이 아니어서 이미지를 화면에 표시할 수 없습니다.")
            print(f"결과 이미지를 확인하세요: {args.output}")
        
    except Exception as e:
        print(f"오류 발생: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()

    
```

추가로 필요한 패키지들을 설치하는 스크립트도 만들어드리겠습니다:​​​​​​​​​​​​​​​​
```bash
#!/bin/bash

echo "박스 감지 프로그램 설치 시작..."

# 시스템 패키지 업데이트
sudo apt update

# Python3 및 pip 설치 (혹시 없다면)
sudo apt install -y python3 python3-pip

# OpenCV 의존성 설치
sudo apt install -y python3-opencv

# 또는 pip로 설치하는 경우:
pip3 install opencv-python numpy

echo "설치 완료!"
echo ""
echo "사용법:"
echo "python3 box_detector.py <이미지파일경로>"
echo "예시: python3 box_detector.py sample.jpg"
```
완성된 프로그램의 주요 기능과 사용법을 설명드리겠습니다:

## 주요 기능

1. **다중 감지 방식**: 엣지 감지와 색상 기반 감지를 결합하여 더 정확한 박스 감지
2. **색상 구별**: 8가지 다른 색상으로 감지된 박스들을 구별하여 표시
3. **중복 제거**: 겹치는 박스들을 자동으로 필터링
4. **좌표 출력**: 각 박스의 (x, y, w, h) 좌표를 콘솔에 출력
5. **결과 저장**: 박스가 그려진 결과 이미지를 파일로 저장

## 설치 방법

```bash
# 설치 스크립트 실행
chmod +x install_requirements.sh
./install_requirements.sh

# 또는 직접 설치
sudo apt update
sudo apt install -y python3 python3-pip python3-opencv
pip3 install opencv-python numpy
```

## 사용법

```bash
# 기본 사용법
python3 box_detector.py image.jpg

# 출력 파일명 지정
python3 box_detector.py image.jpg --output result.jpg
```

## 프로그램 특징

- **적응형 파라미터**: 다양한 크기와 색상의 박스를 감지할 수 있도록 조정 가능
- **노이즈 필터링**: 가우시안 블러와 모폴로지 연산으로 노이즈 제거
- **GUI/CLI 호환**: GUI 환경에서는 이미지를 화면에 표시하고, CLI 환경에서는 파일로만 저장
- **에러 처리**: 파일 존재 여부, 이미지 로드 실패 등의 에러 상황 처리

이 프로그램은 객체 감지(Object Detection)가 적용된 이미지에서 박스들을 찾아 재표시하고 좌표를 출력하는 기능을 제공합니다. 필요에 따라 감지 파라미터들을 조정하여 더 정확한 결과를 얻을 수 있습니다.​​​​​​​​​​​​​​​​
