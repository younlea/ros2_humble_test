우분투에서 동작하는 Python 프로그램을 만들어드리겠습니다. 이 프로그램은 이미지에서 색깔 있는 박스들을 감지하고, 해당 위치에 새로운 박스를 그려주며 좌표를 출력합니다.​​​​​​​​​​​​​​​​

```python
#!/usr/bin/env python3
# 파일: outline_box_color_detector.py

import cv2
import numpy as np
import argparse
import sys
import os

class OutlineColorBoxDetector:
    def __init__(self, min_box_area=100, max_box_area=50000, aspect_ratio_range=(0.1, 10), fill_ratio_threshold=0.5):
        self.min_box_area = min_box_area
        self.max_box_area = max_box_area
        self.aspect_ratio_range = aspect_ratio_range
        self.fill_ratio_threshold = fill_ratio_threshold
        self.color_table = self.get_basic_color_table()

    def preprocess_image(self, img):
        blurred = cv2.GaussianBlur(img, (5, 5), 0)
        return blurred

    def is_valid_box(self, x, y, w, h):
        area = w * h
        aspect_ratio = w / h if h > 0 else 0
        return (self.min_box_area <= area <= self.max_box_area 
                and self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1])

    def is_rectangular_shape(self, contour, tolerance=0.02):
        epsilon = tolerance * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)
        return len(approx) == 4

    def detect_outline_boxes(self, img):
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        edges = cv2.Canny(gray, 50, 150)
        kernel = np.ones((3, 3), np.uint8)
        edges = cv2.dilate(edges, kernel, iterations=1)
        edges = cv2.erode(edges, kernel, iterations=1)

        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        boxes = []

        for contour in contours:
            if not self.is_rectangular_shape(contour):
                continue

            x, y, w, h = cv2.boundingRect(contour)
            if not self.is_valid_box(x, y, w, h):
                continue

            contour_area = cv2.contourArea(contour)
            rect_area = w * h
            fill_ratio = contour_area / rect_area if rect_area > 0 else 0

            if fill_ratio <= self.fill_ratio_threshold:
                boxes.append((x, y, w, h))

        return boxes

    def get_basic_color_table(self):
        return {
            "red": [(0, 50, 50), (10, 255, 255)],
            "orange": [(10, 50, 50), (25, 255, 255)],
            "yellow": [(25, 50, 50), (35, 255, 255)],
            "green": [(35, 50, 50), (85, 255, 255)],
            "cyan": [(85, 50, 50), (100, 255, 255)],
            "blue": [(100, 50, 50), (130, 255, 255)],
            "purple": [(130, 50, 50), (160, 255, 255)],
            "pink": [(160, 50, 50), (180, 255, 255)]
        }

    def classify_box_color(self, img, box):
        x, y, w, h = box

        roi_margin = int(min(w, h) * 0.1)
        roi = img[y+roi_margin:y+h-roi_margin, x+roi_margin:x+w-roi_margin]
        if roi.size == 0:
            return "unknown"

        hsv_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        mean_hsv = np.mean(hsv_roi.reshape(-1, 3), axis=0)

        for color_name, (lower, upper) in self.color_table.items():
            lower_np = np.array(lower)
            upper_np = np.array(upper)
            if np.all(lower_np <= mean_hsv) and np.all(mean_hsv <= upper_np):
                return color_name
        return "unknown"

    def draw_boxes_and_save(self, img, boxes, colors, output_path):
        result_img = img.copy()
        color_map = {
            "red": (0, 0, 255),
            "orange": (0, 165, 255),
            "yellow": (0, 255, 255),
            "green": (0, 255, 0),
            "cyan": (255, 255, 0),
            "blue": (255, 0, 0),
            "purple": (255, 0, 255),
            "pink": (255, 105, 180),
            "unknown": (128, 128, 128)
        }

        for i, (box, color_name) in enumerate(zip(boxes, colors)):
            x, y, w, h = box
            color = color_map.get(color_name, (128, 128, 128))
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            cv2.putText(result_img, f'{i+1}:{color_name}', (x, y-5), cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)

        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='색깔별 외곽선 박스 감지기')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 경로')
    parser.add_argument('--min-area', type=int, default=100)
    parser.add_argument('--max-area', type=int, default=50000)
    parser.add_argument('--min-ratio', type=float, default=0.2)
    parser.add_argument('--max-ratio', type=float, default=5.0)
    parser.add_argument('--fill-ratio', type=float, default=0.5)

    args = parser.parse_args()

    if not os.path.exists(args.image_path):
        print(f"파일 없음: {args.image_path}")
        sys.exit(1)

    img = cv2.imread(args.image_path)
    if img is None:
        print(f"이미지 로드 실패: {args.image_path}")
        sys.exit(1)

    detector = OutlineColorBoxDetector(
        min_box_area=args.min_area,
        max_box_area=args.max_area,
        aspect_ratio_range=(args.min_ratio, args.max_ratio),
        fill_ratio_threshold=args.fill_ratio
    )

    processed_img = detector.preprocess_image(img)
    boxes = detector.detect_outline_boxes(processed_img)

    if not boxes:
        print("박스 없음")
        sys.exit(0)

    colors = [detector.classify_box_color(img, box) for box in boxes]

    for i, (box, color_name) in enumerate(zip(boxes, colors), 1):
        x, y, w, h = box
        print(f"박스 {i}: ({x}, {y}, {w}, {h}) 색상: {color_name}")

    detector.draw_boxes_and_save(img, boxes, colors, args.output)
    print(f"결과 저장됨: {args.output}")

if __name__ == "__main__":
    main()
```

```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램 (Version 4 - 개선된 색상 테이블)
Usage: python box_detector.py <image_path>
"""

import cv2
import numpy as np
import argparse
import sys
import os
from collections import defaultdict

class BoxDetector:
    def __init__(self, min_box_area=100, max_box_area=50000, aspect_ratio_range=(0.1, 10)):
        # 박스 감지를 위한 파라미터 (외부에서 설정 가능)
        self.min_box_area = min_box_area
        self.max_box_area = max_box_area
        self.aspect_ratio_range = aspect_ratio_range
        
        print(f"박스 감지 파라미터:")
        print(f"  - 최소 박스 크기: {self.min_box_area}")
        print(f"  - 최대 박스 크기: {self.max_box_area}")
        print(f"  - 가로세로 비율 범위: {self.aspect_ratio_range}")
        
    def get_precise_color_ranges(self):
        """세밀한 색상 테이블 정의"""
        color_ranges = {
            # 빨간색 계열 - 더 세분화
            'red_bright': [(0, 120, 120), (8, 255, 255)],      # 밝은 빨강
            'red_pure': [(0, 150, 150), (6, 255, 255)],        # 순수 빨강
            'red_dark': [(0, 100, 80), (10, 200, 180)],        # 어두운 빨강
            'red_wrap1': [(170, 120, 120), (179, 255, 255)],   # 빨강 (색상환 끝)
            'red_wrap2': [(175, 150, 150), (179, 255, 255)],   # 진한 빨강 (색상환 끝)
            
            # 주황색 계열
            'orange_bright': [(8, 120, 120), (18, 255, 255)],  # 밝은 주황
            'orange_pure': [(10, 150, 150), (16, 255, 255)],   # 순수 주황
            'orange_red': [(5, 100, 100), (12, 255, 255)],     # 빨강 기운 주황
            'orange_yellow': [(15, 100, 100), (22, 255, 255)], # 노랑 기운 주황
            
            # 노란색 계열
            'yellow_bright': [(18, 120, 120), (28, 255, 255)], # 밝은 노랑
            'yellow_pure': [(20, 150, 150), (26, 255, 255)],   # 순수 노랑
            'yellow_gold': [(15, 100, 100), (25, 200, 200)],   # 금색 노랑
            'yellow_lime': [(25, 120, 120), (35, 255, 255)],   # 라임 노랑
            
            # 초록색 계열 - 더 세분화
            'green_lime': [(28, 120, 120), (45, 255, 255)],    # 라임 초록
            'green_bright': [(40, 120, 120), (70, 255, 255)],  # 밝은 초록
            'green_pure': [(50, 150, 150), (65, 255, 255)],    # 순수 초록
            'green_forest': [(45, 100, 80), (75, 200, 180)],   # 숲 초록
            'green_dark': [(40, 80, 60), (80, 180, 150)],      # 어두운 초록
            'green_emerald': [(55, 120, 120), (75, 255, 255)], # 에메랄드 초록
            
            # 청록색 계열
            'cyan_bright': [(75, 120, 120), (95, 255, 255)],   # 밝은 청록
            'cyan_pure': [(80, 150, 150), (90, 255, 255)],     # 순수 청록
            'cyan_mint': [(70, 80, 120), (100, 180, 255)],     # 민트색
            'teal': [(85, 100, 100), (95, 200, 200)],          # 틸색
            
            # 파란색 계열 - 더 세분화
            'blue_sky': [(95, 120, 120), (110, 255, 255)],     # 하늘색
            'blue_bright': [(100, 120, 120), (125, 255, 255)], # 밝은 파랑
            'blue_pure': [(110, 150, 150), (120, 255, 255)],   # 순수 파랑
            'blue_navy': [(105, 100, 80), (130, 200, 180)],    # 네이비 블루
            'blue_royal': [(115, 120, 120), (125, 255, 255)],  # 로열 블루
            'blue_dark': [(100, 80, 60), (135, 180, 150)],     # 어두운 파랑
            
            # 보라색 계열
            'purple_blue': [(125, 120, 120), (140, 255, 255)], # 파랑 기운 보라
            'purple_bright': [(130, 120, 120), (150, 255, 255)], # 밝은 보라
            'purple_pure': [(135, 150, 150), (145, 255, 255)], # 순수 보라
            'purple_dark': [(125, 100, 80), (155, 200, 180)],  # 어두운 보라
            'violet': [(140, 120, 120), (160, 255, 255)],      # 바이올렛
            
            # 분홍색/마젠타 계열
            'pink_bright': [(145, 120, 120), (165, 255, 255)], # 밝은 분홍
            'pink_hot': [(150, 150, 150), (160, 255, 255)],    # 핫핑크
            'magenta': [(140, 120, 120), (170, 255, 255)],     # 마젠타
            'pink_light': [(145, 80, 150), (170, 180, 255)],   # 연한 분홍
            
            # 갈색 계열 (낮은 채도)
            'brown_red': [(0, 50, 50), (15, 150, 180)],        # 적갈색
            'brown_orange': [(10, 50, 50), (25, 150, 180)],    # 주황갈색
            'brown_yellow': [(20, 50, 50), (35, 150, 180)],    # 황갈색
            'brown_dark': [(5, 30, 30), (25, 120, 120)],       # 어두운 갈색
            
            # 회색 계열 (매우 낮은 채도)
            'gray_light': [(0, 0, 150), (179, 30, 220)],       # 밝은 회색
            'gray_medium': [(0, 0, 100), (179, 30, 180)],      # 중간 회색
            'gray_dark': [(0, 0, 50), (179, 30, 130)],         # 어두운 회색
            
            # 특수 색상들
            'beige': [(15, 30, 120), (30, 80, 220)],           # 베이지
            'cream': [(20, 20, 180), (40, 60, 255)],           # 크림색
            'ivory': [(25, 15, 200), (45, 50, 255)],           # 아이보리
            'khaki': [(35, 40, 100), (55, 120, 200)],          # 카키색
        }
        
        return color_ranges
    
    def preprocess_image(self, img):
        """이미지 전처리 - 더 정교한 노이즈 제거"""
        # 가우시안 블러로 노이즈 제거 (더 부드럽게)
        blurred = cv2.GaussianBlur(img, (5, 5), 0)
        
        # 대비 향상 (CLAHE 적용)
        lab = cv2.cvtColor(blurred, cv2.COLOR_BGR2LAB)
        l, a, b = cv2.split(lab)
        clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
        l = clahe.apply(l)
        enhanced = cv2.merge([l, a, b])
        enhanced = cv2.cvtColor(enhanced, cv2.COLOR_LAB2BGR)
        
        return enhanced
    
    def is_valid_box(self, x, y, w, h):
        """박스 유효성 검사 - 더 엄격한 기준"""
        area = w * h
        aspect_ratio = w / h if h > 0 else 0
        
        # 기본 크기 및 비율 검사
        area_valid = self.min_box_area <= area <= self.max_box_area
        ratio_valid = self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]
        
        # 너무 얇거나 너무 긴 형태 제외
        min_dimension = min(w, h)
        max_dimension = max(w, h)
        
        # 최소 크기 제한 (너무 작은 선 형태 제외)
        dimension_valid = min_dimension >= 5 and max_dimension >= 10
        
        # 극단적인 비율 제외 (매우 얇은 선 형태)
        extreme_ratio_valid = aspect_ratio >= 0.2 and aspect_ratio <= 5.0
        
        return area_valid and ratio_valid and dimension_valid and extreme_ratio_valid
    
    def is_rectangular_shape(self, contour, tolerance=0.1):
        """컨투어가 직사각형 모양인지 확인"""
        # 컨투어를 직사각형으로 근사화
        epsilon = tolerance * cv2.arcLength(contour, True)
        approx = cv2.approxPolyDP(contour, epsilon, True)
        
        # 4개의 꼭짓점을 가져야 함
        if len(approx) != 4:
            return False
        
        # 컨투어 면적과 바운딩 박스 면적 비교
        contour_area = cv2.contourArea(contour)
        x, y, w, h = cv2.boundingRect(contour)
        rect_area = w * h
        
        if rect_area == 0:
            return False
        
        # 면적 비율이 일정 이상이어야 함 (직사각형에 가까워야 함)
        area_ratio = contour_area / rect_area
        return area_ratio > 0.7  # 70% 이상 채워져야 함
    
    def detect_boxes_by_color(self, img):
        """개선된 HSV 색상 기반 박스 감지"""
        boxes = []
        
        # HSV 변환
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 세밀한 색상 범위 가져오기
        color_ranges = self.get_precise_color_ranges()
        
        processed_regions = np.zeros(hsv.shape[:2], dtype=np.uint8)
        
        for color_name, (lower, upper) in color_ranges.items():
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 이미 처리된 영역과 겹치는 부분 제거 (색상 겹침 방지)
            mask = cv2.bitwise_and(mask, cv2.bitwise_not(processed_regions))
            
            if np.sum(mask) < self.min_box_area:  # 너무 작은 영역은 건너뛰기
                continue
            
            # 노이즈 제거 - 더 정교한 모폴로지 연산
            kernel_small = np.ones((3, 3), np.uint8)
            kernel_medium = np.ones((5, 5), np.uint8)
            
            # 작은 노이즈 제거
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel_small)
            # 구멍 메우기
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel_medium)
            # 다시 작은 노이즈 제거
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel_small)
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                # 컨투어가 직사각형 모양인지 확인
                if not self.is_rectangular_shape(contour):
                    continue
                
                x, y, w, h = cv2.boundingRect(contour)
                
                # 유효성 검사
                if self.is_valid_box(x, y, w, h):
                    # 컨투어 면적과 바운딩 박스 면적 비교 (형태 검증)
                    contour_area = cv2.contourArea(contour)
                    rect_area = w * h
                    fill_ratio = contour_area / rect_area if rect_area > 0 else 0
                    
                    # 채움 비율이 적절해야 함 (너무 복잡한 형태 제외)
                    if 0.6 <= fill_ratio <= 0.98:
                        boxes.append((x, y, w, h))
                        # 처리된 영역 마킹 (겹침 방지)
                        cv2.rectangle(processed_regions, (x, y), (x+w, y+h), 255, -1)
        
        return boxes
    
    def detect_boxes_by_edges(self, img):
        """개선된 엣지 기반 박스 감지"""
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # 적응적 임계값을 사용한 엣지 감지
        edges = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                     cv2.THRESH_BINARY, 11, 2)
        
        # Canny 엣지와 결합
        canny_edges = cv2.Canny(gray, 50, 150, apertureSize=3)
        edges = cv2.bitwise_or(edges, canny_edges)
        
        # 모폴로지 연산으로 엣지 연결
        kernel = np.ones((3, 3), np.uint8)
        edges = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel)
        
        # 컨투어 찾기
        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        boxes = []
        for contour in contours:
            # 컨투어가 직사각형 모양인지 확인
            if not self.is_rectangular_shape(contour, tolerance=0.05):
                continue
            
            x, y, w, h = cv2.boundingRect(contour)
            
            # 유효성 검사
            if self.is_valid_box(x, y, w, h):
                boxes.append((x, y, w, h))
        
        return boxes
    
    def remove_duplicate_boxes(self, boxes, overlap_threshold=0.3):
        """중복되는 박스 제거 - 더 엄격한 기준"""
        if not boxes:
            return []
        
        # 면적 기준으로 정렬 (큰 것부터)
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
                
                # 작은 박스 기준으로 겹침 비율 계산
                smaller_area = min(area1, area2)
                if smaller_area > 0:
                    overlap_ratio = overlap_area / smaller_area
                    if overlap_ratio > overlap_threshold:
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
        
        print(f"이미지 크기: {img.shape[1]}x{img.shape[0]}")
        
        # 이미지 전처리
        processed_img = self.preprocess_image(img)
        
        # 색상 기반 박스 감지 (주요 방법)
        print("개선된 HSV 색상 기반 박스 감지 중...")
        color_boxes = self.detect_boxes_by_color(processed_img)
        print(f"색상 방법으로 {len(color_boxes)}개 박스 감지")
        
        # 엣지 기반 박스 감지 (보조 방법)
        print("개선된 엣지 기반 박스 감지 중...")
        edge_boxes = self.detect_boxes_by_edges(processed_img)
        print(f"엣지 방법으로 {len(edge_boxes)}개 박스 감지")
        
        # 모든 박스 합치기
        all_boxes = color_boxes + edge_boxes
        print(f"총 {len(all_boxes)}개 박스 감지 (중복 포함)")
        
        # 중복 제거
        final_boxes = self.remove_duplicate_boxes(all_boxes)
        print(f"중복 제거 후 {len(final_boxes)}개 박스")
        
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
            (0, 128, 255),  # 하늘색
            (255, 192, 203), # 분홍
            (128, 128, 0),  # 올리브
            (0, 128, 128),  # 청록
        ]
        
        for i, (x, y, w, h) in enumerate(boxes):
            color = colors[i % len(colors)]
            
            # 박스 그리기 (굵기 2)
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            
            # 박스 번호 표시
            cv2.putText(result_img, f'{i+1}', (x, y-5), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
            
            # 박스 크기 표시 (선택사항)
            area = w * h
            cv2.putText(result_img, f'{area}px', (x, y+h+15), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, 1)
        
        # 결과 이미지 저장
        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다.')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 파일명 (기본값: output.jpg)')
    parser.add_argument('--min-area', type=int, default=100, help='최소 박스 크기 (기본값: 100)')
    parser.add_argument('--max-area', type=int, default=50000, help='최대 박스 크기 (기본값: 50000)')
    parser.add_argument('--min-ratio', type=float, default=0.2, help='최소 가로세로 비율 (기본값: 0.2)')
    parser.add_argument('--max-ratio', type=float, default=5.0, help='최대 가로세로 비율 (기본값: 5.0)')
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화 (커스텀 파라미터 적용)
        detector = BoxDetector(
            min_box_area=args.min_area,
            max_box_area=args.max_area,
            aspect_ratio_range=(args.min_ratio, args.max_ratio)
        )
        
        print(f"\n이미지 분석 시작: {args.image_path}")
        print("=" * 50)
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            print("파라미터를 조정해보세요:")
            print("  --min-area 값을 줄이거나")
            print("  --max-area 값을 늘려보세요")
            return
        
        # 결과 출력
        print("\n" + "=" * 50)
        print(f"최종 감지된 박스 개수: {len(boxes)}")
        print("-" * 50)
        print("박스 좌표 및 정보:")
        print("형식: 박스번호: (x, y, w, h) - 면적: area")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            area = w * h
            print(f"박스 {i:2d}: ({x:4d}, {y:4d}, {w:4d}, {h:4d}) - 면적: {area:6d}")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original Image', img)
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
```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램 (Version 5)
Usage: python box_detector.py <image_path>
"""

import cv2
import numpy as np
import argparse
import sys
import os
from collections import defaultdict

class BoxDetector:
    def __init__(self, min_box_area=100, max_box_area=50000, aspect_ratio_range=(0.1, 10), iou_threshold=0.5):
        # 박스 감지를 위한 파라미터 (외부에서 설정 가능)
        self.min_box_area = min_box_area
        self.max_box_area = max_box_area
        self.aspect_ratio_range = aspect_ratio_range
        self.iou_threshold = iou_threshold # 중복 박스 제거를 위한 IoU 임계값
        
        print(f"박스 감지 파라미터:")
        print(f"  - 최소 박스 크기: {self.min_box_area}")
        print(f"  - 최대 박스 크기: {self.max_box_area}")
        print(f"  - 가로세로 비율 범위: {self.aspect_ratio_range}")
        print(f"  - 중복 박스 제거 IoU 임계값: {self.iou_threshold}")
        
    def preprocess_image(self, img):
        """이미지 전처리: 가우시안 블러로 노이즈 제거"""
        blurred = cv2.GaussianBlur(img, (5, 5), 0) # 블러 강도 조정 (5,5)
        return blurred
    
    def is_valid_box(self, x, y, w, h, contour_area=None):
        """
        박스 유효성 검사
        - 면적, 가로세로 비율, 사각형 형태 여부 (contour_area가 제공된 경우)
        """
        area = w * h
        aspect_ratio = w / h if h > 0 else 0
        
        area_valid = self.min_box_area <= area <= self.max_box_area
        ratio_valid = self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]
        
        form_valid = True
        if contour_area is not None:
            # 컨투어 면적과 바운딩 박스 면적 비교하여 사각형에 가까운지 확인
            rect_area = w * h
            if rect_area > 0:
                fill_ratio = contour_area / rect_area
                form_valid = fill_ratio > 0.6 and fill_ratio < 1.4 # 사각형에 가까운 비율 (조정 가능)
            else:
                form_valid = False # 면적이 0인 박스는 유효하지 않음
        
        return area_valid and ratio_valid and form_valid
    
    def detect_boxes_by_edges(self, img):
        """엣지 기반 박스 감지: 사각형 형태에 더 집중"""
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # 엣지 감지 (Canny 파라미터 조정 가능)
        edges = cv2.Canny(gray, 70, 200, apertureSize=3) # 엣지 감지 강도 조정
        
        # 모폴로지 연산으로 엣지 연결 및 작은 틈 메우기
        kernel = np.ones((3, 3), np.uint8)
        edges = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel, iterations=2) # 반복 횟수 증가
        edges = cv2.morphologyEx(edges, cv2.MORPH_OPEN, kernel, iterations=1)
        
        # 컨투어 찾기
        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        boxes = []
        for contour in contours:
            # 컨투어를 직사각형으로 근사화 (정확도 향상)
            # arcLength의 2% 이내 오차로 근사화
            epsilon = 0.02 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            
            # 4개의 꼭짓점을 가진 도형 (사각형)만 고려
            if len(approx) == 4:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                
                # 유효성 검사 (면적, 비율, 사각형 형태)
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def analyze_image_colors(self, img, num_hue_segments=12, saturation_threshold=50, value_threshold=50):
        """
        이미지의 주요 색상을 분석하여 동적으로 HSV 색상 범위 생성
        - num_hue_segments: Hue 스펙트럼을 몇 개의 세그먼트로 나눌지 (예: 12는 각 30도)
        - saturation_threshold, value_threshold: 낮은 채도/명도의 색상 (회색 계열) 제외
        """
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        h, w = hsv.shape[:2]
        
        # Hue Histogram 생성 (채도와 명도가 충분히 높은 픽셀만 고려)
        hue_counts = defaultdict(int)
        for y in range(h):
            for x in range(w):
                h_val, s_val, v_val = hsv[y, x]
                if s_val > saturation_threshold and v_val > value_threshold:
                    # Hue를 num_hue_segments개로 분류
                    segment = int(h_val / (180 / num_hue_segments))
                    hue_counts[segment] += 1
        
        dynamic_ranges = []
        # 각 Hue 세그먼트의 대표 색상으로 범위 생성
        for segment, count in hue_counts.items():
            if count > (h * w * 0.001): # 이미지 전체 픽셀의 0.1% 이상 차지하는 색상만 고려
                base_h = segment * (180 / num_hue_segments)
                
                # Hue 범위 설정 (예: +/- 10)
                hue_range = 10
                
                # 빨간색 처리 (0도와 179도 연결)
                if base_h - hue_range < 0: # 0도 이하로 내려가는 경우
                    dynamic_ranges.append([(max(0, int(base_h - hue_range)), saturation_threshold, value_threshold), (int(base_h + hue_range), 255, 255)])
                    dynamic_ranges.append([(int(180 + base_h - hue_range), saturation_threshold, value_threshold), (179, 255, 255)])
                elif base_h + hue_range > 179: # 179도 이상으로 넘어가는 경우
                    dynamic_ranges.append([(int(base_h - hue_range), saturation_threshold, value_threshold), (min(179, int(base_h + hue_range)), 255, 255)])
                    dynamic_ranges.append([(0, saturation_threshold, value_threshold), (int(base_h + hue_range - 180), 255, 255)])
                else:
                    dynamic_ranges.append([(int(base_h - hue_range), saturation_threshold, value_threshold), (int(base_h + hue_range), 255, 255)])
        
        # 중복 범위 제거 및 병합
        merged_ranges = []
        if dynamic_ranges:
            sorted_ranges = sorted(dynamic_ranges, key=lambda x: x[0][0])
            
            # 현재 처리 중인 색상 범위를 리스트로 초기화하여 수정 가능하게 함
            current_lower_list = list(sorted_ranges[0][0]) 
            current_upper_list = list(sorted_ranges[0][1])
            
            for i in range(1, len(sorted_ranges)):
                next_lower_tuple, next_upper_tuple = sorted_ranges[i]
                
                # 현재 범위의 상한 Hue가 다음 범위의 하한 Hue와 겹치거나 인접하면 병합
                # 5는 병합을 위한 여유값
                if current_upper_list[0] >= next_lower_tuple[0] - 5: 
                    # Hue 범위 병합
                    current_upper_list[0] = max(current_upper_list[0], next_upper_tuple[0])
                    
                    # 채도(Saturation)와 명도(Value) 범위 병합 (하한은 최소값, 상한은 최대값)
                    current_lower_list[1] = min(current_lower_list[1], next_lower_tuple[1]) # S_min
                    current_lower_list[2] = min(current_lower_list[2], next_lower_tuple[2]) # V_min
                    current_upper_list[1] = max(current_upper_list[1], next_upper_tuple[1]) # S_max
                    current_upper_list[2] = max(current_upper_list[2], next_upper_tuple[2]) # V_max
                else:
                    # 겹치지 않으면 현재까지 병합된 범위를 추가하고 새로운 범위 시작
                    merged_ranges.append((tuple(current_lower_list), tuple(current_upper_list)))
                    current_lower_list = list(next_lower_tuple)
                    current_upper_list = list(next_upper_tuple)
            
            # 마지막으로 병합된 범위를 추가
            merged_ranges.append((tuple(current_lower_list), tuple(current_upper_list)))

        return merged_ranges
    
    def detect_boxes_by_color(self, img):
        """HSV 색상 기반 박스 감지 (동적 색상 범위 생성)"""
        boxes = []
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 동적 색상 범위 생성
        color_ranges = self.analyze_image_colors(img)
        
        for lower, upper in color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 노이즈 제거 및 객체 연결
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=2) # 2회 반복
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=2) # 2회 반복
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                
                # 박스 형태 검증 (contour_area와 사각형 면적 비교) 및 기타 유효성 검사
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def detect_boxes_by_lab_color(self, img):
        """LAB 색공간을 이용한 색상 기반 박스 감지 (유사한 색상 구분에 유리)"""
        boxes = []
        lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        
        # LAB 색공간에서의 색상 범위들 (L, A, B) - 필요에 따라 조정
        # L (밝기): 0 (검정) ~ 255 (흰색)
        # A (초록-빨강): 0 (초록) ~ 255 (빨강), 128이 중립
        # B (파랑-노랑): 0 (파랑) ~ 255 (노랑), 128이 중립
        lab_color_ranges = [
            # 예시: 빨간색 계열
            [(50, 150, 130), (255, 255, 255)], # 밝은 빨강
            [(30, 130, 120), (200, 255, 170)], # 어두운 빨강
            
            # 예시: 초록색 계열
            [(50, 0, 130), (255, 100, 255)], # 밝은 초록
            [(30, 0, 120), (200, 110, 170)], # 어두운 초록
            
            # 예시: 파란색 계열
            [(50, 130, 0), (255, 170, 120)], # 밝은 파랑
            [(30, 120, 0), (200, 150, 110)], # 어두운 파랑
            
            # 예시: 노란색 계열
            [(50, 120, 150), (255, 140, 255)], # 밝은 노랑
            [(30, 110, 130), (200, 130, 255)], # 어두운 노랑

            # 회색/흰색/검은색 계열 (낮은 A, B 값)
            [(0, 120, 120), (255, 135, 135)], # 회색
            [(200, 120, 120), (255, 135, 135)], # 흰색 (밝기 높음)
            [(0, 120, 120), (50, 135, 135)], # 검은색 (밝기 낮음)
        ]
        
        for lower, upper in lab_color_ranges:
            mask = cv2.inRange(lab, np.array(lower), np.array(upper))
            
            # 노이즈 제거 및 객체 연결
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=1)
            
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def remove_duplicate_boxes(self, boxes):
        """중복되는 박스 제거 (Non-Maximum Suppression 방식)"""
        if not boxes:
            return []
        
        # 박스 좌표를 float32로 변환하여 NMS 입력 형식에 맞춤
        np_boxes = np.array(boxes, dtype=np.float32)
        x1 = np_boxes[:, 0]
        y1 = np_boxes[:, 1]
        x2 = np_boxes[:, 0] + np_boxes[:, 2]
        y2 = np_boxes[:, 1] + np_boxes[:, 3]
        
        # 면적 계산
        areas = np_boxes[:, 2] * np_boxes[:, 3]
        
        # 면적 기준으로 내림차순 정렬
        order = areas.argsort()[::-1]
        
        keep = [] # 최종 선택될 박스의 인덱스
        while order.size > 0:
            i = order[0] # 현재 가장 큰 면적의 박스 선택
            keep.append(i)
            
            # 현재 박스와 다른 박스들의 겹치는 영역 계산
            xx1 = np.maximum(x1[i], x1[order[1:]])
            yy1 = np.maximum(y1[i], y1[order[1:]])
            xx2 = np.minimum(x2[i], x2[order[1:]])
            yy2 = np.minimum(y2[i], y2[order[1:]])
            
            w = np.maximum(0.0, xx2 - xx1 + 1)
            h = np.maximum(0.0, yy2 - yy1 + 1)
            
            overlap = w * h
            
            # IoU 계산
            iou = overlap / (areas[i] + areas[order[1:]] - overlap)
            
            # IoU 임계값보다 작은 박스들만 남김 (겹치지 않는 박스)
            inds = np.where(iou <= self.iou_threshold)[0]
            order = order[inds + 1] # 다음 반복을 위해 인덱스 업데이트
        
        final_boxes = [boxes[i] for i in keep]
        return final_boxes
    
    def detect_boxes(self, img_path):
        """메인 박스 감지 함수"""
        # 이미지 로드
        img = cv2.imread(img_path)
        if img is None:
            raise ValueError(f"이미지를 로드할 수 없습니다: {img_path}")
        
        print(f"이미지 크기: {img.shape[1]}x{img.shape[0]}")
        
        # 이미지 전처리
        processed_img = self.preprocess_image(img)
        
        # 다양한 방법으로 박스 감지
        print("엣지 기반 박스 감지 중 (사각형 형태 집중)...")
        edge_boxes = self.detect_boxes_by_edges(processed_img)
        print(f"  엣지 방법으로 {len(edge_boxes)}개 박스 감지")
        
        print("HSV 색상 기반 박스 감지 중 (동적 색상 범위)...")
        hsv_color_boxes = self.detect_boxes_by_color(processed_img)
        print(f"  HSV 색상 방법으로 {len(hsv_color_boxes)}개 박스 감지")
        
        print("LAB 색상 기반 박스 감지 중 (유사색상 구분)...")
        lab_color_boxes = self.detect_boxes_by_lab_color(processed_img)
        print(f"  LAB 색상 방법으로 {len(lab_color_boxes)}개 박스 감지")
        
        # 모든 박스 합치기
        all_boxes = edge_boxes + hsv_color_boxes + lab_color_boxes
        print(f"총 {len(all_boxes)}개 박스 감지 (중복 포함)")
        
        # 중복 제거
        final_boxes = self.remove_duplicate_boxes(all_boxes)
        print(f"중복 제거 후 {len(final_boxes)}개 박스")
        
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
            (0, 128, 255),  # 하늘색
            (255, 192, 203), # 분홍
            (128, 128, 0),  # 올리브
            (0, 128, 128),  # 청록
            (0, 100, 0),    # 진한 초록
            (100, 0, 0),    # 진한 파랑
            (0, 0, 100),    # 진한 빨강
        ]
        
        for i, (x, y, w, h) in enumerate(boxes):
            color = colors[i % len(colors)]
            
            # 박스 그리기 (굵기 2)
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            
            # 박스 번호 표시
            cv2.putText(result_img, f'{i+1}', (x, y-5), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
            
            # 박스 크기 표시 (선택사항)
            area = w * h
            cv2.putText(result_img, f'{w}x{h} ({area})', (x, y+h+15), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, 1)
        
        # 결과 이미지 저장
        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다.')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 파일명 (기본값: output.jpg)')
    parser.add_argument('--min-area', type=int, default=100, help='최소 박스 크기 (기본값: 100)')
    parser.add_argument('--max-area', type=int, default=50000, help='최대 박스 크기 (기본값: 50000)')
    parser.add_argument('--min-ratio', type=float, default=0.2, help='최소 가로세로 비율 (기본값: 0.2)') # 비율 조정
    parser.add_argument('--max-ratio', type=float, default=5.0, help='최대 가로세로 비율 (기본값: 5.0)')   # 비율 조정
    parser.add_argument('--iou-threshold', type=float, default=0.3, help='중복 박스 제거 IoU 임계값 (기본값: 0.3)') # IoU 임계값 조정
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화 (커스텀 파라미터 적용)
        detector = BoxDetector(
            min_box_area=args.min_area,
            max_box_area=args.max_area,
            aspect_ratio_range=(args.min_ratio, args.max_ratio),
            iou_threshold=args.iou_threshold
        )
        
        print(f"\n이미지 분석 시작: {args.image_path}")
        print("=" * 50)
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            print("파라미터를 조정해보세요:")
            print("  --min-area 값을 줄이거나")
            print("  --max-area 값을 늘리거나")
            print("  --min-ratio/--max-ratio 값을 조정하거나")
            print("  --iou-threshold 값을 조정해보세요.")
            return
        
        # 결과 출력
        print("\n" + "=" * 50)
        print(f"최종 감지된 박스 개수: {len(boxes)}")
        print("-" * 50)
        print("박스 좌표 및 정보:")
        print("형식: 박스번호: (x, y, w, h) - 면적: area (가로x세로)")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            area = w * h
            print(f"박스 {i:2d}: ({x:4d}, {y:4d}, {w:4d}, {h:4d}) - {w}x{h} ({area:6d})")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original Image', img)
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

```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램 (Version 4)
Usage: python box_detector.py <image_path>
"""

import cv2
import numpy as np
import argparse
import sys
import os
from collections import defaultdict

class BoxDetector:
    def __init__(self, min_box_area=100, max_box_area=50000, aspect_ratio_range=(0.1, 10), iou_threshold=0.5):
        # 박스 감지를 위한 파라미터 (외부에서 설정 가능)
        self.min_box_area = min_box_area
        self.max_box_area = max_box_area
        self.aspect_ratio_range = aspect_ratio_range
        self.iou_threshold = iou_threshold # 중복 박스 제거를 위한 IoU 임계값
        
        print(f"박스 감지 파라미터:")
        print(f"  - 최소 박스 크기: {self.min_box_area}")
        print(f"  - 최대 박스 크기: {self.max_box_area}")
        print(f"  - 가로세로 비율 범위: {self.aspect_ratio_range}")
        print(f"  - 중복 박스 제거 IoU 임계값: {self.iou_threshold}")
        
    def preprocess_image(self, img):
        """이미지 전처리: 가우시안 블러로 노이즈 제거"""
        blurred = cv2.GaussianBlur(img, (5, 5), 0) # 블러 강도 조정 (5,5)
        return blurred
    
    def is_valid_box(self, x, y, w, h, contour_area=None):
        """
        박스 유효성 검사
        - 면적, 가로세로 비율, 사각형 형태 여부 (contour_area가 제공된 경우)
        """
        area = w * h
        aspect_ratio = w / h if h > 0 else 0
        
        area_valid = self.min_box_area <= area <= self.max_box_area
        ratio_valid = self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]
        
        form_valid = True
        if contour_area is not None:
            # 컨투어 면적과 바운딩 박스 면적 비교하여 사각형에 가까운지 확인
            rect_area = w * h
            if rect_area > 0:
                fill_ratio = contour_area / rect_area
                form_valid = fill_ratio > 0.6 and fill_ratio < 1.4 # 사각형에 가까운 비율 (조정 가능)
            else:
                form_valid = False # 면적이 0인 박스는 유효하지 않음
        
        return area_valid and ratio_valid and form_valid
    
    def detect_boxes_by_edges(self, img):
        """엣지 기반 박스 감지: 사각형 형태에 더 집중"""
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # 엣지 감지 (Canny 파라미터 조정 가능)
        edges = cv2.Canny(gray, 70, 200, apertureSize=3) # 엣지 감지 강도 조정
        
        # 모폴로지 연산으로 엣지 연결 및 작은 틈 메우기
        kernel = np.ones((3, 3), np.uint8)
        edges = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel, iterations=2) # 반복 횟수 증가
        edges = cv2.morphologyEx(edges, cv2.MORPH_OPEN, kernel, iterations=1)
        
        # 컨투어 찾기
        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        boxes = []
        for contour in contours:
            # 컨투어를 직사각형으로 근사화 (정확도 향상)
            # arcLength의 2% 이내 오차로 근사화
            epsilon = 0.02 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            
            # 4개의 꼭짓점을 가진 도형 (사각형)만 고려
            if len(approx) == 4:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                
                # 유효성 검사 (면적, 비율, 사각형 형태)
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def analyze_image_colors(self, img, num_hue_segments=12, saturation_threshold=50, value_threshold=50):
        """
        이미지의 주요 색상을 분석하여 동적으로 HSV 색상 범위 생성
        - num_hue_segments: Hue 스펙트럼을 몇 개의 세그먼트로 나눌지 (예: 12는 각 30도)
        - saturation_threshold, value_threshold: 낮은 채도/명도의 색상 (회색 계열) 제외
        """
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        h, w = hsv.shape[:2]
        
        # Hue Histogram 생성 (채도와 명도가 충분히 높은 픽셀만 고려)
        hue_counts = defaultdict(int)
        for y in range(h):
            for x in range(w):
                h_val, s_val, v_val = hsv[y, x]
                if s_val > saturation_threshold and v_val > value_threshold:
                    # Hue를 num_hue_segments개로 분류
                    segment = int(h_val / (180 / num_hue_segments))
                    hue_counts[segment] += 1
        
        dynamic_ranges = []
        # 각 Hue 세그먼트의 대표 색상으로 범위 생성
        for segment, count in hue_counts.items():
            if count > (h * w * 0.001): # 이미지 전체 픽셀의 0.1% 이상 차지하는 색상만 고려
                base_h = segment * (180 / num_hue_segments)
                
                # Hue 범위 설정 (예: +/- 10)
                hue_range = 10
                
                # 빨간색 처리 (0도와 179도 연결)
                if base_h - hue_range < 0: # 0도 이하로 내려가는 경우
                    dynamic_ranges.append([(max(0, int(base_h - hue_range)), saturation_threshold, value_threshold), (int(base_h + hue_range), 255, 255)])
                    dynamic_ranges.append([(int(180 + base_h - hue_range), saturation_threshold, value_threshold), (179, 255, 255)])
                elif base_h + hue_range > 179: # 179도 이상으로 넘어가는 경우
                    dynamic_ranges.append([(int(base_h - hue_range), saturation_threshold, value_threshold), (min(179, int(base_h + hue_range)), 255, 255)])
                    dynamic_ranges.append([(0, saturation_threshold, value_threshold), (int(base_h + hue_range - 180), 255, 255)])
                else:
                    dynamic_ranges.append([(int(base_h - hue_range), saturation_threshold, value_threshold), (int(base_h + hue_range), 255, 255)])
        
        # 중복 범위 제거 및 병합 (간단한 형태)
        merged_ranges = []
        if dynamic_ranges:
            sorted_ranges = sorted(dynamic_ranges, key=lambda x: x[0][0])
            current_range = list(sorted_ranges[0])
            
            for i in range(1, len(sorted_ranges)):
                next_range = sorted_ranges[i]
                # 현재 범위의 상한 Hue가 다음 범위의 하한 Hue와 겹치거나 인접하면 병합
                if current_range[1][0] >= next_range[0][0] - 5: # 5는 병합을 위한 여유값
                    current_range[1][0] = max(current_range[1][0], next_range[1][0])
                    current_range[0][1] = min(current_range[0][1], next_range[0][1]) # Smin
                    current_range[0][2] = min(current_range[0][2], next_range[0][2]) # Vmin
                    current_range[1][1] = max(current_range[1][1], next_range[1][1]) # Smax
                    current_range[1][2] = max(current_range[1][2], next_range[1][2]) # Vmax
                else:
                    merged_ranges.append(tuple(current_range))
                    current_range = list(next_range)
            merged_ranges.append(tuple(current_range))

        return merged_ranges
    
    def detect_boxes_by_color(self, img):
        """HSV 색상 기반 박스 감지 (동적 색상 범위 생성)"""
        boxes = []
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        
        # 동적 색상 범위 생성
        color_ranges = self.analyze_image_colors(img)
        
        for lower, upper in color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 노이즈 제거 및 객체 연결
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=2) # 2회 반복
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=2) # 2회 반복
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                
                # 박스 형태 검증 (contour_area와 사각형 면적 비교) 및 기타 유효성 검사
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def detect_boxes_by_lab_color(self, img):
        """LAB 색공간을 이용한 색상 기반 박스 감지 (유사한 색상 구분에 유리)"""
        boxes = []
        lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        
        # LAB 색공간에서의 색상 범위들 (L, A, B) - 필요에 따라 조정
        # L (밝기): 0 (검정) ~ 255 (흰색)
        # A (초록-빨강): 0 (초록) ~ 255 (빨강), 128이 중립
        # B (파랑-노랑): 0 (파랑) ~ 255 (노랑), 128이 중립
        lab_color_ranges = [
            # 예시: 빨간색 계열
            [(50, 150, 130), (255, 255, 255)], # 밝은 빨강
            [(30, 130, 120), (200, 255, 170)], # 어두운 빨강
            
            # 예시: 초록색 계열
            [(50, 0, 130), (255, 100, 255)], # 밝은 초록
            [(30, 0, 120), (200, 110, 170)], # 어두운 초록
            
            # 예시: 파란색 계열
            [(50, 130, 0), (255, 170, 120)], # 밝은 파랑
            [(30, 120, 0), (200, 150, 110)], # 어두운 파랑
            
            # 예시: 노란색 계열
            [(50, 120, 150), (255, 140, 255)], # 밝은 노랑
            [(30, 110, 130), (200, 130, 255)], # 어두운 노랑

            # 회색/흰색/검은색 계열 (낮은 A, B 값)
            [(0, 120, 120), (255, 135, 135)], # 회색
            [(200, 120, 120), (255, 135, 135)], # 흰색 (밝기 높음)
            [(0, 120, 120), (50, 135, 135)], # 검은색 (밝기 낮음)
        ]
        
        for lower, upper in lab_color_ranges:
            mask = cv2.inRange(lab, np.array(lower), np.array(upper))
            
            # 노이즈 제거 및 객체 연결
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel, iterations=1)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel, iterations=1)
            
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                contour_area = cv2.contourArea(contour)
                if self.is_valid_box(x, y, w, h, contour_area=contour_area):
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def remove_duplicate_boxes(self, boxes):
        """중복되는 박스 제거 (Non-Maximum Suppression 방식)"""
        if not boxes:
            return []
        
        # 박스 좌표를 float32로 변환하여 NMS 입력 형식에 맞춤
        np_boxes = np.array(boxes, dtype=np.float32)
        x1 = np_boxes[:, 0]
        y1 = np_boxes[:, 1]
        x2 = np_boxes[:, 0] + np_boxes[:, 2]
        y2 = np_boxes[:, 1] + np_boxes[:, 3]
        
        # 면적 계산
        areas = np_boxes[:, 2] * np_boxes[:, 3]
        
        # 면적 기준으로 내림차순 정렬
        order = areas.argsort()[::-1]
        
        keep = [] # 최종 선택될 박스의 인덱스
        while order.size > 0:
            i = order[0] # 현재 가장 큰 면적의 박스 선택
            keep.append(i)
            
            # 현재 박스와 다른 박스들의 겹치는 영역 계산
            xx1 = np.maximum(x1[i], x1[order[1:]])
            yy1 = np.maximum(y1[i], y1[order[1:]])
            xx2 = np.minimum(x2[i], x2[order[1:]])
            yy2 = np.minimum(y2[i], y2[order[1:]])
            
            w = np.maximum(0.0, xx2 - xx1 + 1)
            h = np.maximum(0.0, yy2 - yy1 + 1)
            
            overlap = w * h
            
            # IoU 계산
            iou = overlap / (areas[i] + areas[order[1:]] - overlap)
            
            # IoU 임계값보다 작은 박스들만 남김 (겹치지 않는 박스)
            inds = np.where(iou <= self.iou_threshold)[0]
            order = order[inds + 1] # 다음 반복을 위해 인덱스 업데이트
        
        final_boxes = [boxes[i] for i in keep]
        return final_boxes
    
    def detect_boxes(self, img_path):
        """메인 박스 감지 함수"""
        # 이미지 로드
        img = cv2.imread(img_path)
        if img is None:
            raise ValueError(f"이미지를 로드할 수 없습니다: {img_path}")
        
        print(f"이미지 크기: {img.shape[1]}x{img.shape[0]}")
        
        # 이미지 전처리
        processed_img = self.preprocess_image(img)
        
        # 다양한 방법으로 박스 감지
        print("엣지 기반 박스 감지 중 (사각형 형태 집중)...")
        edge_boxes = self.detect_boxes_by_edges(processed_img)
        print(f"  엣지 방법으로 {len(edge_boxes)}개 박스 감지")
        
        print("HSV 색상 기반 박스 감지 중 (동적 색상 범위)...")
        hsv_color_boxes = self.detect_boxes_by_color(processed_img)
        print(f"  HSV 색상 방법으로 {len(hsv_color_boxes)}개 박스 감지")
        
        print("LAB 색상 기반 박스 감지 중 (유사색상 구분)...")
        lab_color_boxes = self.detect_boxes_by_lab_color(processed_img)
        print(f"  LAB 색상 방법으로 {len(lab_color_boxes)}개 박스 감지")
        
        # 모든 박스 합치기
        all_boxes = edge_boxes + hsv_color_boxes + lab_color_boxes
        print(f"총 {len(all_boxes)}개 박스 감지 (중복 포함)")
        
        # 중복 제거
        final_boxes = self.remove_duplicate_boxes(all_boxes)
        print(f"중복 제거 후 {len(final_boxes)}개 박스")
        
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
            (0, 128, 255),  # 하늘색
            (255, 192, 203), # 분홍
            (128, 128, 0),  # 올리브
            (0, 128, 128),  # 청록
            (0, 100, 0),    # 진한 초록
            (100, 0, 0),    # 진한 파랑
            (0, 0, 100),    # 진한 빨강
        ]
        
        for i, (x, y, w, h) in enumerate(boxes):
            color = colors[i % len(colors)]
            
            # 박스 그리기 (굵기 2)
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            
            # 박스 번호 표시
            cv2.putText(result_img, f'{i+1}', (x, y-5), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
            
            # 박스 크기 표시 (선택사항)
            area = w * h
            cv2.putText(result_img, f'{w}x{h} ({area})', (x, y+h+15), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, 1)
        
        # 결과 이미지 저장
        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다.')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 파일명 (기본값: output.jpg)')
    parser.add_argument('--min-area', type=int, default=100, help='최소 박스 크기 (기본값: 100)')
    parser.add_argument('--max-area', type=int, default=50000, help='최대 박스 크기 (기본값: 50000)')
    parser.add_argument('--min-ratio', type=float, default=0.2, help='최소 가로세로 비율 (기본값: 0.2)') # 비율 조정
    parser.add_argument('--max-ratio', type=float, default=5.0, help='최대 가로세로 비율 (기본값: 5.0)')   # 비율 조정
    parser.add_argument('--iou-threshold', type=float, default=0.3, help='중복 박스 제거 IoU 임계값 (기본값: 0.3)') # IoU 임계값 조정
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화 (커스텀 파라미터 적용)
        detector = BoxDetector(
            min_box_area=args.min_area,
            max_box_area=args.max_area,
            aspect_ratio_range=(args.min_ratio, args.max_ratio),
            iou_threshold=args.iou_threshold
        )
        
        print(f"\n이미지 분석 시작: {args.image_path}")
        print("=" * 50)
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            print("파라미터를 조정해보세요:")
            print("  --min-area 값을 줄이거나")
            print("  --max-area 값을 늘리거나")
            print("  --min-ratio/--max-ratio 값을 조정하거나")
            print("  --iou-threshold 값을 조정해보세요.")
            return
        
        # 결과 출력
        print("\n" + "=" * 50)
        print(f"최종 감지된 박스 개수: {len(boxes)}")
        print("-" * 50)
        print("박스 좌표 및 정보:")
        print("형식: 박스번호: (x, y, w, h) - 면적: area (가로x세로)")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            area = w * h
            print(f"박스 {i:2d}: ({x:4d}, {y:4d}, {w:4d}, {h:4d}) - {w}x{h} ({area:6d})")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original Image', img)
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

```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램 (Version 3)
Usage: python box_detector.py <image_path>
"""

import cv2
import numpy as np
import argparse
import sys
import os
from collections import defaultdict

class BoxDetector:
    def __init__(self, min_box_area=100, max_box_area=50000, aspect_ratio_range=(0.1, 10)):
        # 박스 감지를 위한 파라미터 (외부에서 설정 가능)
        self.min_box_area = min_box_area
        self.max_box_area = max_box_area
        self.aspect_ratio_range = aspect_ratio_range
        
        print(f"박스 감지 파라미터:")
        print(f"  - 최소 박스 크기: {self.min_box_area}")
        print(f"  - 최대 박스 크기: {self.max_box_area}")
        print(f"  - 가로세로 비율 범위: {self.aspect_ratio_range}")
        
    def preprocess_image(self, img):
        """이미지 전처리"""
        # 가우시안 블러로 노이즈 제거
        blurred = cv2.GaussianBlur(img, (3, 3), 0)
        return blurred
    
    def is_valid_box(self, x, y, w, h):
        """박스 유효성 검사"""
        area = w * h
        aspect_ratio = w / h if h > 0 else 0
        
        area_valid = self.min_box_area <= area <= self.max_box_area
        ratio_valid = self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1]
        
        return area_valid and ratio_valid
    
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
                
                # 유효성 검사
                if self.is_valid_box(x, y, w, h):
                    boxes.append((x, y, w, h))
        
        return boxes
    
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
        """HSV 색상 기반 박스 감지 (정적 + 동적 색상 범위)"""
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
        
        for lower, upper in all_color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 노이즈 제거 - 더 적극적인 필터링
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
            
            # 작은 노이즈 제거를 위한 추가 필터링
            kernel2 = np.ones((5, 5), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel2)
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                
                # 박스 형태 검증 - 컨투어가 직사각형에 가까운지 확인
                contour_area = cv2.contourArea(contour)
                rect_area = w * h
                fill_ratio = contour_area / rect_area if rect_area > 0 else 0
                
                # 유효성 검사 (크기, 비율, 형태)
                if self.is_valid_box(x, y, w, h) and fill_ratio > 0.3:
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def detect_boxes_by_lab_color(self, img):
        """LAB 색공간을 이용한 색상 기반 박스 감지"""
        boxes = []
        
        # LAB 색공간으로 변환
        lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        
        # LAB 색공간에서의 색상 범위들 (L, A, B)
        lab_color_ranges = [
            # 빨간색 계열 (높은 A 값)
            [(0, 130, 120), (255, 255, 255)],
            [(20, 140, 130), (255, 255, 255)],
            
            # 초록색 계열 (낮은 A 값)
            [(0, 0, 0), (255, 120, 255)],
            [(20, 0, 0), (255, 110, 255)],
            
            # 파란색 계열 (낮은 B 값)
            [(0, 0, 0), (255, 255, 120)],
            [(20, 0, 0), (255, 255, 110)],
            
            # 노란색 계열 (높은 B 값)
            [(0, 0, 130), (255, 255, 255)],
            [(20, 0, 140), (255, 255, 255)],
            
            # 보라/마젠타 계열 (높은 A, 낮은 B)
            [(0, 130, 0), (255, 255, 120)],
            
            # 청록 계열 (낮은 A, 높은 B)
            [(0, 0, 130), (255, 120, 255)],
        ]
        
        for lower, upper in lab_color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(lab, np.array(lower), np.array(upper))
            
            # 노이즈 제거
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                
                # 유효성 검사
                if self.is_valid_box(x, y, w, h):
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
        
        print(f"이미지 크기: {img.shape[1]}x{img.shape[0]}")
        
        # 이미지 전처리
        processed_img = self.preprocess_image(img)
        
        # 다양한 방법으로 박스 감지
        print("엣지 기반 박스 감지 중...")
        edge_boxes = self.detect_boxes_by_edges(processed_img)
        print(f"엣지 방법으로 {len(edge_boxes)}개 박스 감지")
        
        print("HSV 색상 기반 박스 감지 중...")
        hsv_color_boxes = self.detect_boxes_by_color(processed_img)
        print(f"HSV 색상 방법으로 {len(hsv_color_boxes)}개 박스 감지")
        
        print("LAB 색상 기반 박스 감지 중...")
        lab_color_boxes = self.detect_boxes_by_lab_color(processed_img)
        print(f"LAB 색상 방법으로 {len(lab_color_boxes)}개 박스 감지")
        
        # 모든 박스 합치기
        all_boxes = edge_boxes + hsv_color_boxes + lab_color_boxes
        print(f"총 {len(all_boxes)}개 박스 감지 (중복 포함)")
        
        # 중복 제거
        final_boxes = self.remove_duplicate_boxes(all_boxes)
        print(f"중복 제거 후 {len(final_boxes)}개 박스")
        
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
            (0, 128, 255),  # 하늘색
            (255, 192, 203), # 분홍
            (128, 128, 0),  # 올리브
            (0, 128, 128),  # 청록
        ]
        
        for i, (x, y, w, h) in enumerate(boxes):
            color = colors[i % len(colors)]
            
            # 박스 그리기 (굵기 2)
            cv2.rectangle(result_img, (x, y), (x + w, y + h), color, 2)
            
            # 박스 번호 표시
            cv2.putText(result_img, f'{i+1}', (x, y-5), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
            
            # 박스 크기 표시 (선택사항)
            area = w * h
            cv2.putText(result_img, f'{area}px', (x, y+h+15), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, 1)
        
        # 결과 이미지 저장
        cv2.imwrite(output_path, result_img)
        return result_img

def main():
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다.')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output.jpg', help='출력 이미지 파일명 (기본값: output.jpg)')
    parser.add_argument('--min-area', type=int, default=100, help='최소 박스 크기 (기본값: 100)')
    parser.add_argument('--max-area', type=int, default=50000, help='최대 박스 크기 (기본값: 50000)')
    parser.add_argument('--min-ratio', type=float, default=0.1, help='최소 가로세로 비율 (기본값: 0.1)')
    parser.add_argument('--max-ratio', type=float, default=10.0, help='최대 가로세로 비율 (기본값: 10.0)')
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화 (커스텀 파라미터 적용)
        detector = BoxDetector(
            min_box_area=args.min_area,
            max_box_area=args.max_area,
            aspect_ratio_range=(args.min_ratio, args.max_ratio)
        )
        
        print(f"\n이미지 분석 시작: {args.image_path}")
        print("=" * 50)
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            print("파라미터를 조정해보세요:")
            print("  --min-area 값을 줄이거나")
            print("  --max-area 값을 늘려보세요")
            return
        
        # 결과 출력
        print("\n" + "=" * 50)
        print(f"최종 감지된 박스 개수: {len(boxes)}")
        print("-" * 50)
        print("박스 좌표 및 정보:")
        print("형식: 박스번호: (x, y, w, h) - 면적: area")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            area = w * h
            print(f"박스 {i:2d}: ({x:4d}, {y:4d}, {w:4d}, {h:4d}) - 면적: {area:6d}")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original Image', img)
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

```python
#!/usr/bin/env python3
"""
이미지에서 컬러 박스를 감지하고 좌표를 출력하는 프로그램 (Version 3)
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
        
        for lower, upper in all_color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(hsv, np.array(lower), np.array(upper))
            
            # 노이즈 제거 - 더 적극적인 필터링
            kernel = np.ones((3, 3), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
            
            # 작은 노이즈 제거를 위한 추가 필터링
            kernel2 = np.ones((5, 5), np.uint8)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel2)
            
            # 컨투어 찾기
            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            for contour in contours:
                x, y, w, h = cv2.boundingRect(contour)
                area = w * h
                aspect_ratio = w / h if h > 0 else 0
                
                # 박스 형태 검증 - 컨투어가 직사각형에 가까운지 확인
                contour_area = cv2.contourArea(contour)
                rect_area = w * h
                fill_ratio = contour_area / rect_area if rect_area > 0 else 0
                
                if (self.min_box_area <= area <= self.max_box_area and 
                    self.aspect_ratio_range[0] <= aspect_ratio <= self.aspect_ratio_range[1] and
                    fill_ratio > 0.3):  # 박스 형태 검증
                    boxes.append((x, y, w, h))
        
        return boxes
    
    def detect_boxes_by_lab_color(self, img):
        """LAB 색공간을 이용한 색상 기반 박스 감지"""
        boxes = []
        
        # LAB 색공간으로 변환
        lab = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
        
        # LAB 색공간에서의 색상 범위들 (L, A, B)
        lab_color_ranges = [
            # 빨간색 계열 (높은 A 값)
            [(0, 130, 120), (255, 255, 255)],
            [(20, 140, 130), (255, 255, 255)],
            
            # 초록색 계열 (낮은 A 값)
            [(0, 0, 0), (255, 120, 255)],
            [(20, 0, 0), (255, 110, 255)],
            
            # 파란색 계열 (낮은 B 값)
            [(0, 0, 0), (255, 255, 120)],
            [(20, 0, 0), (255, 255, 110)],
            
            # 노란색 계열 (높은 B 값)
            [(0, 0, 130), (255, 255, 255)],
            [(20, 0, 140), (255, 255, 255)],
            
            # 보라/마젠타 계열 (높은 A, 낮은 B)
            [(0, 130, 0), (255, 255, 120)],
            
            # 청록 계열 (낮은 A, 높은 B)
            [(0, 0, 130), (255, 120, 255)],
        ]
        
        for lower, upper in lab_color_ranges:
            # 색상 마스크 생성
            mask = cv2.inRange(lab, np.array(lower), np.array(upper))
            
            # 노이즈 제거
            kernel = np.ones((3, 3), np.uint8)
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
        hsv_color_boxes = self.detect_boxes_by_color(processed_img)
        lab_color_boxes = self.detect_boxes_by_lab_color(processed_img)
        
        # 모든 박스 합치기
        all_boxes = edge_boxes + hsv_color_boxes + lab_color_boxes
        
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
            (0, 128, 255),  # 연한 파랑
            (128, 255, 0),  # 연두
            (255, 128, 0),  # 진한 주황
            (0, 255, 128),  # 민트
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
    parser = argparse.ArgumentParser(description='이미지에서 박스를 감지하고 좌표를 출력합니다. (Version 3)')
    parser.add_argument('image_path', help='입력 이미지 경로')
    parser.add_argument('--output', '-o', default='output_v3.jpg', help='출력 이미지 파일명 (기본값: output_v3.jpg)')
    parser.add_argument('--min-area', type=int, default=100, help='최소 박스 크기 (기본값: 100)')
    parser.add_argument('--max-area', type=int, default=50000, help='최대 박스 크기 (기본값: 50000)')
    parser.add_argument('--verbose', '-v', action='store_true', help='상세 정보 출력')
    
    args = parser.parse_args()
    
    # 입력 파일 존재 확인
    if not os.path.exists(args.image_path):
        print(f"오류: 파일을 찾을 수 없습니다 - {args.image_path}")
        sys.exit(1)
    
    try:
        # 박스 감지기 초기화
        detector = BoxDetector()
        detector.min_box_area = args.min_area
        detector.max_box_area = args.max_area
        
        print(f"이미지 분석 중: {args.image_path}")
        if args.verbose:
            print(f"설정: 최소 크기={args.min_area}, 최대 크기={args.max_area}")
        
        # 박스 감지
        img, boxes = detector.detect_boxes(args.image_path)
        
        if not boxes:
            print("감지된 박스가 없습니다.")
            print("팁: --min-area 값을 줄이거나 --max-area 값을 늘려보세요.")
            return
        
        # 결과 출력
        print(f"\n감지된 박스 개수: {len(boxes)}")
        print("-" * 60)
        print("박스 좌표 (x, y, w, h) - 좌상단 좌표와 너비/높이:")
        
        for i, (x, y, w, h) in enumerate(boxes, 1):
            print(f"박스 {i:2d}: ({x:4d}, {y:4d}, {w:4d}, {h:4d}) - 면적: {w*h:6d}")
            if args.verbose:
                print(f"        우하단: ({x+w:4d}, {y+h:4d}), 비율: {w/h:.2f}")
        
        # 박스 그리고 결과 이미지 저장
        result_img = detector.draw_boxes_and_save(img, boxes, args.output)
        print(f"\n결과 이미지가 저장되었습니다: {args.output}")
        
        # 이미지 표시 (GUI 환경에서만)
        try:
            cv2.imshow('Original Image', img)
            cv2.imshow('Detected Boxes', result_img)
            print("\n이미지 창이 열렸습니다. 아무 키나 눌러 종료하세요.")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        except cv2.error:
            print("GUI 환경이 아니어서 이미지를 화면에 표시할 수 없습니다.")
            print(f"결과 이미지를 확인하세요: {args.output}")
        
    except Exception as e:
        print(f"오류 발생: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
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
