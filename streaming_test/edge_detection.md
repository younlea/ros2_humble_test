이 문제를 해결하기 위해 POI 영역에서 특정 패턴(은색 바트)을 감지하고, 엣지를 추적하며 움직임을 분석하는 방법을 사용할 수 있습니다. 아래는 이를 구현하기 위한 단계와 코드 예제입니다.

단계별 접근 방법

1. 영상 데이터 전처리
	•	POI 영역만 추출: 설정된 POI 영역을 자릅니다.
	•	그레이스케일 변환: 엣지 디텍션은 명암 차이에 기반하므로 색상 정보를 제거합니다.
	•	노이즈 제거: GaussianBlur 같은 필터를 사용해 작은 노이즈를 제거합니다.

2. 엣지 감지
	•	OpenCV의 Canny 엣지 디텍터를 사용합니다.
	•	이진 엣지 맵을 생성하여 관심 영역의 윤곽선을 추출합니다.

3. 특정 물체(은색 바트) 검출
	•	바트의 크기, 형태, 색상 등의 특성을 기반으로 ROI 내에서 바트를 감지합니다.
	•	윤곽선(Contour)을 추적하여 바트와 일치하는 객체를 필터링합니다.

4. 움직임 감지
	•	이전 프레임과 현재 프레임에서 바트의 위치를 비교하여 이동 거리를 계산합니다.

코드 구현

아래는 이 과정을 구현한 Python 코드입니다.

```python
import cv2
import numpy as np

class ObjectTracker:
    def __init__(self, poi_rect):
        self.poi_rect = poi_rect  # POI 범위 (x, y, w, h)
        self.prev_position = None  # 이전 바트 중심점
        self.current_position = None  # 현재 바트 중심점

    def process_frame(self, frame):
        """
        입력 프레임에서 POI 영역만 처리하고, 바트를 감지하고 이동 여부를 반환.
        """
        # POI 영역 추출
        x, y, w, h = self.poi_rect
        poi_frame = frame[y:y+h, x:x+w]

        # 그레이스케일 변환
        gray = cv2.cvtColor(poi_frame, cv2.COLOR_BGR2GRAY)

        # 블러링으로 노이즈 제거
        blurred = cv2.GaussianBlur(gray, (5, 5), 0)

        # 엣지 검출
        edges = cv2.Canny(blurred, 50, 150)

        # 윤곽선 감지
        contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        # 특정 크기 범위의 윤곽선만 필터링 (바트 크기와 일치)
        detected_objects = []
        for contour in contours:
            area = cv2.contourArea(contour)
            if 1000 < area < 5000:  # 가로 40 x 세로 30에 근접한 면적 (크기 조정 가능)
                x, y, w, h = cv2.boundingRect(contour)
                aspect_ratio = w / h
                if 1.2 < aspect_ratio < 2.0:  # 바트의 가로 세로 비율 제한
                    detected_objects.append((x, y, w, h))

        # 바트가 감지되었을 때 처리
        if detected_objects:
            # 첫 번째 객체 선택 (여러 바트를 감지하는 경우 우선 하나만 추적)
            x, y, w, h = detected_objects[0]
            cx, cy = x + w // 2, y + h // 2  # 바트 중심점 계산

            self.current_position = (cx, cy)

            # 이동 여부 감지
            if self.prev_position:
                dx = cx - self.prev_position[0]
                dy = cy - self.prev_position[1]
                print(f"Movement detected: dx={dx}, dy={dy}")

            # 업데이트
            self.prev_position = self.current_position

            # POI 영역에 바트 표시
            cv2.rectangle(poi_frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(poi_frame, "Detected", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

        # 결과 이미지 반환 (디버깅용)
        return poi_frame

# 테스트 코드
def main():
    cap = cv2.VideoCapture("rtsp://192.168.0.4")  # RTSP 스트림 URL
    poi_rect = (100, 100, 400, 300)  # POI 범위 (x, y, width, height)
    tracker = ObjectTracker(poi_rect)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # POI 내에서 바트 추적
        processed_frame = tracker.process_frame(frame)

        # 결과 표시
        cv2.imshow("POI Frame", processed_frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
```
주요 구현 내용
	1.	POI 영역만 처리:
	•	프레임에서 self.poi_rect로 지정된 영역만 잘라내어 처리 속도를 높였습니다.
	2.	엣지 디텍션:
	•	cv2.Canny로 윤곽선을 감지하고, 크기와 형태를 기준으로 특정 객체(바트)를 필터링합니다.
	3.	바트 중심점 추적:
	•	바트의 중심 좌표를 계산하여 이전 프레임과 비교하고 이동 거리를 계산합니다.
	4.	결과 표시:
	•	바트가 감지되면 녹색 사각형으로 표시하고, 이동 여부를 출력합니다.

수정 가능한 부분
	1.	바트 크기와 비율:
	•	if 1000 < area < 5000 및 if 1.2 < aspect_ratio < 2.0 부분을 바트의 실제 크기와 비율에 맞게 조정하세요.
	2.	ROI 설정:
	•	poi_rect 값은 실제 POI 영역에 맞게 조정하세요.

이 코드는 기본적인 엣지 기반 바트 감지와 이동 추적을 구현합니다. 정확도를 더 높이고 싶다면 바트의 색상, 텍스처, 또는 더 정교한 모델(예: YOLO, OpenCV DNN)로 객체 검출을 추가적으로 구현할 수 있습니다.
