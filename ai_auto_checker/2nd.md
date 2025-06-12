import cv2
import numpy as np
import os
from glob import glob
import shutil

# --- 설정 ---
input_folder = './input_images'
output_folder = './filtered_images'
os.makedirs(output_folder, exist_ok=True)
image_files = glob(os.path.join(input_folder, '*[102].jpg'))

# --- 전역 변수 ---
roi1 = roi2 = ref_box = None
roi1_selected = roi2_selected = ref_box_selected = False
mode = None
ix, iy = -1, -1
current_img_idx = 0
stop_debug = False
message = ""

# 해상도 자동 조정용
display_scale = 1.0
max_width = 1600
max_height = 900

# --- 버튼 설정 ---
button_height = 50
button_width = 150
button_margin = 10
button_color = (220, 220, 220)
button_active_color = (180, 180, 255)
font_color = (0, 0, 0)

button_names = ['ROI 1', 'ROI 2', 'Reference Box', 'Start', 'DEBUG', 'NEXT', 'STOP']
buttons = {}
for idx, name in enumerate(button_names):
    buttons[name] = (
        button_margin + idx * (button_width + button_margin),
        button_margin,
        button_width,
        button_height
    )

def resize_image(img):
    global display_scale
    h, w = img.shape[:2]
    scale_w = max_width / w
    scale_h = max_height / h
    display_scale = min(scale_w, scale_h, 1.0)
    if display_scale < 1.0:
        img = cv2.resize(img, (int(w * display_scale), int(h * display_scale)))
    return img

def to_original_coord(x, y):
    return int(x / display_scale), int(y / display_scale)

def draw_buttons(img, active=None):
    for name, (x, y, w, h) in buttons.items():
        color = button_active_color if name == active else button_color
        cv2.rectangle(img, (x, y), (x + w, y + h), color, -1)
        cv2.putText(img, name, (x + 15, y + 35), cv2.FONT_HERSHEY_SIMPLEX, 0.8, font_color, 2)

def draw_message(img):
    cv2.rectangle(img, (0, max_height-50), (max_width, max_height), (50,50,50), -1)
    cv2.putText(img, message, (20, max_height-15), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255,255,255), 2)

def draw_boxes(img):
    for box, color, label in [
        (roi1, (0, 255, 0), 'ROI 1'),
        (roi2, (255, 0, 0), 'ROI 2'),
        (ref_box, (0, 0, 255), 'Reference Box')
    ]:
        if box:
            x, y, w, h = box
            x, y, w, h = [int(i * display_scale) for i in (x, y, w, h)]
            cv2.rectangle(img, (x, y), (x + w, y + h), color, 2)
            cv2.putText(img, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)

def check_boxes(img, roi, ref_area, min_ratio, max_ratio, draw=False):
    x, y, w, h = roi
    roi_img = img[y:y+h, x:x+w]
    gray = cv2.cvtColor(roi_img, cv2.COLOR_BGR2GRAY)
    _, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    found = False
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area < 100: continue
        x0, y0, w0, h0 = cv2.boundingRect(cnt)
        box_area = w0 * h0
        if draw:
            cv2.rectangle(roi_img, (x0, y0), (x0 + w0, y0 + h0), (0, 255, 255), 2)
            cv2.putText(roi_img, f"{box_area:.0f}", (x0, y0-5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,255), 1)
        if box_area > ref_area * max_ratio or box_area < ref_area * min_ratio:
            found = True
    if draw:
        img[y:y+h, x:x+w] = roi_img
    return found

def mouse_callback(event, x, y, flags, param):
    global ix, iy, roi1, roi2, ref_box
    global roi1_selected, roi2_selected, ref_box_selected, mode, stop_debug

    ox, oy = to_original_coord(x, y)

    if event == cv2.EVENT_LBUTTONDOWN:
        for name, (bx, by, bw, bh) in buttons.items():
            if bx <= x <= bx + bw and by <= y <= by + bh:
                mode = name.lower()
                if name == 'STOP':
                    stop_debug = True
                return
        ix, iy = ox, oy

    elif event == cv2.EVENT_LBUTTONUP:
        ex, ey = ox, oy
        box = (min(ix, ex), min(iy, ey), abs(ex - ix), abs(ey - iy))
        if mode == 'roi 1':
            roi1, roi1_selected = box, True
        elif mode == 'roi 2':
            roi2, roi2_selected = box, True
        elif mode == 'reference box':
            ref_box, ref_box_selected = box, True

# --- 시작 ---
if len(image_files) == 0:
    print('No images found.')
    exit()

img0 = cv2.imread(image_files[0])
clone = img0.copy()

print('기준 박스 대비 최소 비율 (예: 0.5): ', end='')
min_ratio = float(input())
print('기준 박스 대비 최대 비율 (예: 1.5): ', end='')
max_ratio = float(input())

cv2.namedWindow('Image', cv2.WINDOW_NORMAL)
cv2.setMouseCallback('Image', mouse_callback)

while True:
    disp = resize_image(clone.copy())
    draw_buttons(disp, active=mode)
    draw_boxes(disp)
    draw_message(disp)
    cv2.imshow('Image', disp)

    key = cv2.waitKey(10) & 0xFF
    if key == 27:
        break

    if mode == 'start':
        if not (roi1 and roi2 and ref_box):
            message = '모든 박스를 지정하세요!'
            mode = None
            continue
        ref_area = ref_box[2] * ref_box[3]
        for img_path in image_files:
            img = cv2.imread(img_path)
            flag = any(check_boxes(img, roi, ref_area, min_ratio, max_ratio) for roi in [roi1, roi2])
            if flag:
                shutil.copy(img_path, output_folder)
        message = '분류 완료!'
        mode = None

    elif mode == 'debug':
        if not (roi1 and roi2 and ref_box):
            message = '모든 박스를 지정하세요!'
            mode = None
            continue
        ref_area = ref_box[2] * ref_box[3]
        current_img_idx, stop_debug = 0, False
        mode = None

        while current_img_idx < len(image_files):
            img_path = image_files[current_img_idx]
            img = cv2.imread(img_path)
            img_debug = img.copy()

            found = any(check_boxes(img_debug, roi, ref_area, min_ratio, max_ratio, draw=True) for roi in [roi1, roi2])
            draw_boxes(img_debug)
            status = "Found" if found else "OK"
            message = f"[{current_img_idx+1}/{len(image_files)}] {os.path.basename(img_path)} → {status}"

            while True:
                disp_debug = resize_image(img_debug.copy())
                draw_buttons(disp_debug)
                draw_message(disp_debug)
                cv2.imshow('Image', disp_debug)

                key2 = cv2.waitKey(10) & 0xFF
                if stop_debug:
                    break
                if mode == 'next':
                    mode = None
                    break

            if stop_debug:
                break
            current_img_idx += 1

cv2.destroyAllWindows()
