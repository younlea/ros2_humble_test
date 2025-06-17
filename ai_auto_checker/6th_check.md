
{}(code )

아래는 요청하신 대로 Python과 Tkinter를 사용해 Ubuntu에서 동작하는 GUI 애플리케이션의 예제 코드입니다. 이 프로그램은 특정 폴더에서 [102].jpg 파일을 읽고, 사용자가 ROI(Region of Interest)를 선택한 후, 이미지를 순차적으로 보여주며 체크된 파일을 지정된 폴더에 저장합니다. OpenCV와 PIL을 사용해 이미지 처리를 수행합니다.
요구 사항
```
	•	Python 3.x
	•	필요한 라이브러리: opencv-python, Pillow, tkinter pip install opencv-python Pillow
	•	
```
코드
```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        self.scale_factor = 1.0
        self.base_width = 160
        self.base_height = 60
        self.roi_combined_size = None
        self.selected_images = {}  # 선택 상태 저장 {img_path: bool}

        # GUI 요소 (2줄 레이아웃)
        # 1줄: 폴더 선택, 저장 폴더 선택, ROI 선택
        self.top_frame = tk.Frame(root)
        self.top_frame.pack(pady=5, fill=tk.X)

        self.btn_select_folder = tk.Button(self.top_frame, text="Select Input Folder", command=self.select_folder)
        self.btn_select_folder.pack(side=tk.LEFT, padx=5)

        self.btn_select_save_folder = tk.Button(self.top_frame, text="Select Save Folder", command=self.select_save_folder, state=tk.DISABLED)
        self.btn_select_save_folder.pack(side=tk.LEFT, padx=5)

        self.label_folder = tk.Label(self.top_frame, text="No folder selected")
        self.label_folder.pack(side=tk.LEFT, padx=5)

        self.btn_roi1 = tk.Button(self.top_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.top_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.top_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.top_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 2줄: 스타트, 이전/다음/줌, 상태
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5, fill=tk.X)

        self.btn_start = tk.Button(self.button_frame, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(side=tk.LEFT, padx=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_in = tk.Button(self.button_frame, text="+", command=self.zoom_in, state=tk.DISABLED)
        self.btn_zoom_in.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_out = tk.Button(self.button_frame, text="−", command=self.zoom_out, state=tk.DISABLED)
        self.btn_zoom_out.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 3x4 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        for i in range(4):  # 4행
            for j in range(3):  # 3열
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=10, pady=10)
                canvas = tk.Canvas(frame, width=self.base_width, height=self.base_height, bg="gray")
                canvas.pack()
                canvas.bind("<Button-1>", lambda e, idx=i*3+j: self.toggle_selection(idx))
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)

        self.image_label = None
        self.grid_images = []

    def toggle_selection(self, idx):
        """이미지 클릭 시 선택/해제"""
        if idx < len(self.grid_images):
            img_path = self.grid_images[idx]
            self.check_vars[idx].set(not self.check_vars[idx].get())
            self.selected_images[img_path] = self.check_vars[idx].get()
            logging.info(f"Toggled selection for image {img_path}: {self.check_vars[idx].get()}")
            self.update_canvas_overlay(idx)

    def update_canvas_overlay(self, idx):
        """선택 상태에 따라 캔버스에 오버레이 업데이트"""
        canvas = self.grid_canvases[idx]
        canvas.delete("overlay")
        if self.check_vars[idx].get():
            canvas.create_rectangle(
                0, 0, canvas.winfo_width(), canvas.winfo_height(),
                fill="blue", stipple="gray50", tags="overlay"
            )

    def select_folder(self):
        logging.info("Selecting input folder")
        self.folder_path = filedialog.askdirectory(title="Select Input Folder")
        if self.folder_path:
            # 기본 저장 폴더 설정
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Input: {os.path.basename(self.folder_path)} | Save: {os.path.basename(self.save_folder)}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.scale_factor = 1.0
                self.selected_images = {}  # 선택 상태 초기화
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.btn_zoom_in.config(state=tk.DISABLED)
                self.btn_zoom_out.config(state=tk.DISABLED)
                self.btn_select_save_folder.config(state=tk.NORMAL)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_select_save_folder.config(state=tk.DISABLED)

    def select_save_folder(self):
        logging.info("Selecting save folder")
        new_save_folder = filedialog.askdirectory(title="Select Save Folder")
        if new_save_folder:
            self.save_folder = new_save_folder
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Input: {os.path.basename(self.folder_path)} | Save: {os.path.basename(self.save_folder)}")
            logging.info(f"Save folder set to: {self.save_folder}")

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        if self.selected_roi == mode:
            # 동일 ROI 재클릭 시 토글 해제 및 초기화
            self.selected_roi = None
            if mode == "ROI1":
                self.roi1 = None
                self.roi_count = 0
                self.canvas.delete("roi1")
                self.label_roi1.config(text="ROI1: Not selected")
                self.btn_roi1.config(relief=tk.RAISED)
                self.btn_roi2.config(state=tk.DISABLED)
            elif mode == "ROI2":
                self.roi2 = None
                self.roi_count = 1
                self.canvas.delete("roi2")
                self.label_roi2.config(text="ROI2: Not selected")
                self.btn_roi2.config(relief=tk.RAISED)
                self.btn_roi1.config(state=tk.NORMAL)
        else:
            # 새 ROI 선택
            self.selected_roi = mode
            self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
            self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_folder.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(relief=tk.RAISED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(relief=tk.RAISED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        img = cv2.imread(self.image_files[0])
        if img is None:
            logging.error("Failed to load first image for ROI sizing")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

        # 연결된 이미지 크기 최적화
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            h, w = combined_img.shape[:2]
            grid_width = 1200 - 3 * 20 - 20  # 3열, padx=10, 여백 20
            grid_height = 800 - 100 - 4 * 20 - 20  # 상단 100, 4행, pady=10, 여백 20
            cell_width = grid_width // 3
            cell_height = grid_height // 4
            scale = min(cell_width/w, cell_height/h, 1.0)
            self.base_width = int(w * scale)
            self.base_height = int(h * scale)
            self.roi_combined_size = (self.base_width, self.base_height)
            logging.info(f"Optimized ROI combined size: {self.roi_combined_size}")
        except cv2.error as e:
            logging.error(f"Error calculating ROI size: {e}")
            self.roi_combined_size = (160, 60)
            self.base_width = 160
            self.base_height = 60

        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.btn_zoom_in.config(state=tk.NORMAL)
        self.btn_zoom_out.config(state=tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def zoom_in(self):
        if self.scale_factor < 4.0:
            self.scale_factor += 0.1
            logging.info(f"Zoom in: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def zoom_out(self):
        if self.scale_factor > 0.25:
            self.scale_factor -= 0.1
            logging.info(f"Zoom out: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def update_grid_size(self):
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        font_size = max(6, int(8 * self.scale_factor))
        wraplength = int(150 * self.scale_factor)

        for canvas in self.grid_canvases:
            canvas.config(width=new_width, height=new_height)
        for label in self.grid_labels:
            label.config(font=("Arial", font_size), wraplength=wraplength)

        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        for var in self.check_vars:
            var.set(False)
        for canvas in self.grid_canvases:
            canvas.delete("overlay")

        self.grid_images = []
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (new_width, new_height), interpolation=cv2.INTER_CUBIC)
                self.display_image(self.grid_canvases[i], combined_img)
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
                # 선택 상태 복원
                self.check_vars[i].set(self.selected_images.get(img_path, False))
                self.update_canvas_overlay(i)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")

        # 상태 업데이트
        remaining_pages = math.ceil((len(self.image_files) - start_idx) / self.images_per_page) - 1
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)} (남은 페이지: {remaining_pages})")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)

    def save_selected_images(self):
        logging.info("Saving selected images")
        for img_path, selected in self.selected_images.items():
            if selected:
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                crop_name = f"[crop]{base_name}"
                crop_dst_path = os.path.join(self.save_folder, crop_name)

                # 원본 이미지 저장
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in save folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in save folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                        continue
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")
                    continue

                # 크롭 이미지 저장
                try:
                    img = cv2.imread(img_path)
                    if img is None:
                        logging.error(f"Failed to load image for cropping: {img_path}")
                        continue
                    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                    h, w = img.shape[:2]

                    # ROI1 좌표
                    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
                    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
                    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
                    if x2 <= x1 or y2 <= y1:
                        logging.warning(f"Invalid ROI1 for cropping: {img_path}")
                        continue
                    roi1_img = img[y1:y2, x1:x2]

                    # ROI2 좌표
                    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
                    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
                    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
                    if x2 <= x1 or y2 <= y1:
                        logging.warning(f"Invalid ROI2 for cropping: {img_path}")
                        continue
                    roi2_img = img[y1:y2, x1:x2]

                    # 높이 맞추기
                    h1, w1 = roi1_img.shape[:2]
                    h2, w2 = roi2_img.shape[:2]
                    max_h = max(h1, h2)
                    if h1 < max_h:
                        padding = max_h - h1
                        roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
                    elif h2 < max_h:
                        padding = max_h - h2
                        roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

                    # 이미지 연결 및 저장
                    combined_img = cv2.hconcat([roi1_img, roi2_img])
                    combined_img = cv2.cvtColor(combined_img, cv2.COLOR_RGB2BGR)
                    if os.path.exists(crop_dst_path):
                        logging.warning(f"Crop file {crop_name} already exists in save folder")
                        messagebox.showwarning("Warning", f"Crop file {crop_name} already exists in save folder!")
                        continue
                    cv2.imwrite(crop_dst_path, combined_img)
                    logging.info(f"Saved cropped image {crop_name} to {self.save_folder}")
                except Exception as e:
                    logging.error(f"Error saving cropped image {crop_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save cropped image {crop_name}: {e}")

    def prev_image(self):
        self.current_page -= 1
        self.show_roi_images()

    def next_image(self):
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            self.save_selected_images()
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.btn_zoom_in.config(state=tk.DISABLED)
            self.btn_zoom_out.config(state=tk.DISABLED)
            self.btn_select_save_folder.config(state=tk.DISABLED)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            self.selected_images = {}  # 선택 상태 초기화
            self.scale_factor = 1.0
            self.base_width = 160
            self.base_height = 60
            for canvas in self.grid_canvases:
                canvas.delete("all")
                canvas.config(width=160, height=60)
            for label in self.grid_labels:
                label.config(text="", font=("Arial", 8), wraplength=150)
            for var in self.check_vars:
                var.set(False)
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == '__main__':
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        self.scale_factor = 1.0
        self.base_width = 160
        self.base_height = 60
        self.roi_combined_size = None

        # GUI 요소 (2줄 레이아웃)
        # 1줄: 폴더 선택, 저장 폴더 선택, ROI 선택
        self.top_frame = tk.Frame(root)
        self.top_frame.pack(pady=5, fill=tk.X)

        self.btn_select_folder = tk.Button(self.top_frame, text="Select Input Folder", command=self.select_folder)
        self.btn_select_folder.pack(side=tk.LEFT, padx=5)

        self.btn_select_save_folder = tk.Button(self.top_frame, text="Select Save Folder", command=self.select_save_folder, state=tk.DISABLED)
        self.btn_select_save_folder.pack(side=tk.LEFT, padx=5)

        self.label_folder = tk.Label(self.top_frame, text="No folder selected")
        self.label_folder.pack(side=tk.LEFT, padx=5)

        self.btn_roi1 = tk.Button(self.top_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.top_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.top_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.top_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 2줄: 스타트, 이전/다음/줌, 상태
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5, fill=tk.X)

        self.btn_start = tk.Button(self.button_frame, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(side=tk.LEFT, padx=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_in = tk.Button(self.button_frame, text="+", command=self.zoom_in, state=tk.DISABLED)
        self.btn_zoom_in.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_out = tk.Button(self.button_frame, text="−", command=self.zoom_out, state=tk.DISABLED)
        self.btn_zoom_out.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 3x4 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        for i in range(4):  # 4행
            for j in range(3):  # 3열
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=10, pady=10)
                canvas = tk.Canvas(frame, width=self.base_width, height=self.base_height, bg="gray")
                canvas.pack()
                canvas.bind("<Button-1>", lambda e, idx=i*3+j: self.toggle_selection(idx))
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)

        self.image_label = None
        self.grid_images = []

    def toggle_selection(self, idx):
        """이미지 클릭 시 선택/해제"""
        self.check_vars[idx].set(not self.check_vars[idx].get())
        logging.info(f"Toggled selection for image {idx}: {self.check_vars[idx].get()}")
        self.update_canvas_overlay(idx)

    def update_canvas_overlay(self, idx):
        """선택 상태에 따라 캔버스에 오버레이 업데이트"""
        canvas = self.grid_canvases[idx]
        canvas.delete("overlay")
        if self.check_vars[idx].get():
            canvas.create_rectangle(
                0, 0, canvas.winfo_width(), canvas.winfo_height(),
                fill="blue", stipple="gray50", tags="overlay"
            )

    def select_folder(self):
        logging.info("Selecting input folder")
        self.folder_path = filedialog.askdirectory(title="Select Input Folder")
        if self.folder_path:
            # 기본 저장 폴더 설정
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Input: {os.path.basename(self.folder_path)} | Save: {os.path.basename(self.save_folder)}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.scale_factor = 1.0
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.btn_zoom_in.config(state=tk.DISABLED)
                self.btn_zoom_out.config(state=tk.DISABLED)
                self.btn_select_save_folder.config(state=tk.NORMAL)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_select_save_folder.config(state=tk.DISABLED)

    def select_save_folder(self):
        logging.info("Selecting save folder")
        new_save_folder = filedialog.askdirectory(title="Select Save Folder")
        if new_save_folder:
            self.save_folder = new_save_folder
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Input: {os.path.basename(self.folder_path)} | Save: {os.path.basename(self.save_folder)}")
            logging.info(f"Save folder set to: {self.save_folder}")

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        if self.selected_roi == mode:
            # 동일 ROI 재클릭 시 토글 해제 및 초기화
            self.selected_roi = None
            if mode == "ROI1":
                self.roi1 = None
                self.roi_count = 0
                self.canvas.delete("roi1")
                self.label_roi1.config(text="ROI1: Not selected")
                self.btn_roi1.config(relief=tk.RAISED)
                self.btn_roi2.config(state=tk.DISABLED)
            elif mode == "ROI2":
                self.roi2 = None
                self.roi_count = 1
                self.canvas.delete("roi2")
                self.label_roi2.config(text="ROI2: Not selected")
                self.btn_roi2.config(relief=tk.RAISED)
                self.btn_roi1.config(state=tk.NORMAL)
        else:
            # 새 ROI 선택
            self.selected_roi = mode
            self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
            self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_folder.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(relief=tk.RAISED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(relief=tk.RAISED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        img = cv2.imread(self.image_files[0])
        if img is None:
            logging.error("Failed to load first image for ROI sizing")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

        # 연결된 이미지 크기 최적화
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            h, w = combined_img.shape[:2]
            grid_width = 1200 - 3 * 20 - 20  # 3열, padx=10, 여백 20
            grid_height = 800 - 100 - 4 * 20 - 20  # 상단 100, 4행, pady=10, 여백 20
            cell_width = grid_width // 3
            cell_height = grid_height // 4
            scale = min(cell_width/w, cell_height/h, 1.0)
            self.base_width = int(w * scale)
            self.base_height = int(h * scale)
            self.roi_combined_size = (self.base_width, self.base_height)
            logging.info(f"Optimized ROI combined size: {self.roi_combined_size}")
        except cv2.error as e:
            logging.error(f"Error calculating ROI size: {e}")
            self.roi_combined_size = (160, 60)
            self.base_width = 160
            self.base_height = 60

        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.btn_zoom_in.config(state=tk.NORMAL)
        self.btn_zoom_out.config(state=tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def zoom_in(self):
        if self.scale_factor < 4.0:
            self.scale_factor += 0.1
            logging.info(f"Zoom in: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def zoom_out(self):
        if self.scale_factor > 0.25:
            self.scale_factor -= 0.1
            logging.info(f"Zoom out: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def update_grid_size(self):
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        font_size = max(6, int(8 * self.scale_factor))
        wraplength = int(150 * self.scale_factor)

        for canvas in self.grid_canvases:
            canvas.config(width=new_width, height=new_height)
        for label in self.grid_labels:
            label.config(font=("Arial", font_size), wraplength=wraplength)

        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        for var in self.check_vars:
            var.set(False)
        for canvas in self.grid_canvases:
            canvas.delete("overlay")

        self.grid_images = []
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (new_width, new_height), interpolation=cv2.INTER_CUBIC)
                self.display_image(self.grid_canvases[i], combined_img)
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
                self.update_canvas_overlay(i)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")

        # 상태 업데이트
        remaining_pages = math.ceil((len(self.image_files) - start_idx) / self.images_per_page) - 1
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)} (남은 페이지: {remaining_pages})")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)

    def save_selected_images(self):
        logging.info("Saving selected images")
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                crop_name = f"[crop]{base_name}"
                crop_dst_path = os.path.join(self.save_folder, crop_name)

                # 원본 이미지 저장
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in save folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in save folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                        continue
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")
                    continue

                # 크롭 이미지 저장
                try:
                    img = cv2.imread(img_path)
                    if img is None:
                        logging.error(f"Failed to load image for cropping: {img_path}")
                        continue
                    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                    h, w = img.shape[:2]

                    # ROI1 좌표
                    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
                    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
                    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
                    if x2 <= x1 or y2 <= y1:
                        logging.warning(f"Invalid ROI1 for cropping: {img_path}")
                        continue
                    roi1_img = img[y1:y2, x1:x2]

                    # ROI2 좌표
                    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
                    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
                    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
                    if x2 <= x1 or y2 <= y1:
                        logging.warning(f"Invalid ROI2 for cropping: {img_path}")
                        continue
                    roi2_img = img[y1:y2, x1:x2]

                    # 높이 맞추기
                    h1, w1 = roi1_img.shape[:2]
                    h2, w2 = roi2_img.shape[:2]
                    max_h = max(h1, h2)
                    if h1 < max_h:
                        padding = max_h - h1
                        roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
                    elif h2 < max_h:
                        padding = max_h - h2
                        roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

                    # 이미지 연결 및 저장
                    combined_img = cv2.hconcat([roi1_img, roi2_img])
                    combined_img = cv2.cvtColor(combined_img, cv2.COLOR_RGB2BGR)
                    if os.path.exists(crop_dst_path):
                        logging.warning(f"Crop file {crop_name} already exists in save folder")
                        messagebox.showwarning("Warning", f"Crop file {crop_name} already exists in save folder!")
                        continue
                    cv2.imwrite(crop_dst_path, combined_img)
                    logging.info(f"Saved cropped image {crop_name} to {self.save_folder}")
                except Exception as e:
                    logging.error(f"Error saving cropped image {crop_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save cropped image {crop_name}: {e}")

    def prev_image(self):
        self.save_selected_images()
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        self.save_selected_images()
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.btn_zoom_in.config(state=tk.DISABLED)
            self.btn_zoom_out.config(state=tk.DISABLED)
            self.btn_select_save_folder.config(state=tk.DISABLED)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            self.scale_factor = 1.0
            self.base_width = 160
            self.base_height = 60
            for canvas in self.grid_canvases:
                canvas.delete("all")
                canvas.config(width=160, height=60)
            for label in self.grid_labels:
                label.config(text="", font=("Arial", 8), wraplength=150)
            for var in self.check_vars:
                var.set(False)
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```


```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        self.scale_factor = 1.0
        self.base_width = 160
        self.base_height = 60
        self.roi_combined_size = None

        # GUI 요소 (2줄 레이아웃)
        # 1줄: 폴더 선택, ROI 선택
        self.top_frame = tk.Frame(root)
        self.top_frame.pack(pady=5, fill=tk.X)

        self.btn_select_folder = tk.Button(self.top_frame, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(side=tk.LEFT, padx=5)

        self.label_folder = tk.Label(self.top_frame, text="No folder selected")
        self.label_folder.pack(side=tk.LEFT, padx=5)

        self.btn_roi1 = tk.Button(self.top_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.top_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.top_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.top_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 2줄: 스타트, 이전/다음/줌, 상태
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5, fill=tk.X)

        self.btn_start = tk.Button(self.button_frame, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(side=tk.LEFT, padx=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_in = tk.Button(self.button_frame, text="+", command=self.zoom_in, state=tk.DISABLED)
        self.btn_zoom_in.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_out = tk.Button(self.button_frame, text="−", command=self.zoom_out, state=tk.DISABLED)
        self.btn_zoom_out.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 3x4 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        for i in range(4):  # 4행
            for j in range(3):  # 3열
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=10, pady=10)
                canvas = tk.Canvas(frame, width=self.base_width, height=self.base_height, bg="gray")
                canvas.pack()
                canvas.bind("<Button-1>", lambda e, idx=i*3+j: self.toggle_selection(idx))
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)

        self.image_label = None
        self.grid_images = []

    def toggle_selection(self, idx):
        """이미지 클릭 시 선택/해제"""
        self.check_vars[idx].set(not self.check_vars[idx].get())
        logging.info(f"Toggled selection for image {idx}: {self.check_vars[idx].get()}")
        self.update_canvas_overlay(idx)

    def update_canvas_overlay(self, idx):
        """선택 상태에 따라 캔버스에 오버레이 업데이트"""
        canvas = self.grid_canvases[idx]
        canvas.delete("overlay")
        if self.check_vars[idx].get():
            canvas.create_rectangle(
                0, 0, canvas.winfo_width(), canvas.winfo_height(),
                fill="blue", stipple="gray50", tags="overlay"
            )

    def select_folder(self):
        logging.info("Selecting folder")
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Selected Folder: {os.path.basename(self.folder_path)}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.scale_factor = 1.0
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.btn_zoom_in.config(state=tk.DISABLED)
                self.btn_zoom_out.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_folder.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        img = cv2.imread(self.image_files[0])
        if img is None:
            logging.error("Failed to load first image for ROI sizing")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

        # 연결된 이미지 크기 최적화
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            h, w = combined_img.shape[:2]
            # 3x4 그리드에 맞게 크기 조정 (창 크기: 1200x800)
            grid_width = 1200 - 3 * 20 - 20  # 3열, padx=10, 여백 20
            grid_height = 800 - 100 - 4 * 20 - 20  # 상단 100, 4행, pady=10, 여백 20
            cell_width = grid_width // 3
            cell_height = grid_height // 4
            scale = min(cell_width/w, cell_height/h, 1.0)
            self.base_width = int(w * scale)
            self.base_height = int(h * scale)
            self.roi_combined_size = (self.base_width, self.base_height)
            logging.info(f"Optimized ROI combined size: {self.roi_combined_size}")
        except cv2.error as e:
            logging.error(f"Error calculating ROI size: {e}")
            self.roi_combined_size = (160, 60)
            self.base_width = 160
            self.base_height = 60

        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.btn_zoom_in.config(state=tk.NORMAL)
        self.btn_zoom_out.config(state=tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def zoom_in(self):
        if self.scale_factor < 4.0:
            self.scale_factor += 0.1
            logging.info(f"Zoom in: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def zoom_out(self):
        if self.scale_factor > 0.25:
            self.scale_factor -= 0.1
            logging.info(f"Zoom out: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def update_grid_size(self):
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        font_size = max(6, int(8 * self.scale_factor))
        wraplength = int(150 * self.scale_factor)

        for canvas in self.grid_canvases:
            canvas.config(width=new_width, height=new_height)
        for label in self.grid_labels:
            label.config(font=("Arial", font_size), wraplength=wraplength)

        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        for var in self.check_vars:
            var.set(False)
        for canvas in self.grid_canvases:
            canvas.delete("overlay")

        self.grid_images = []
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (new_width, new_height), interpolation=cv2.INTER_CUBIC)
                self.display_image(self.grid_canvases[i], combined_img)
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
                self.update_canvas_overlay(i)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")

        # 상태 업데이트
        remaining_pages = math.ceil((len(self.image_files) - start_idx) / self.images_per_page) - 1
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)} (남은 페이지: {remaining_pages})")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)

    def save_selected_images(self):
        logging.info("Saving selected images")
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in detected_folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in detected_folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")

    def prev_image(self):
        self.save_selected_images()
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        self.save_selected_images()
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.btn_zoom_in.config(state=tk.DISABLED)
            self.btn_zoom_out.config(state=tk.DISABLED)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            self.scale_factor = 1.0
            self.base_width = 160
            self.base_height = 60
            for canvas in self.grid_canvases:
                canvas.delete("all")
                canvas.config(width=160, height=60)
            for label in self.grid_labels:
                label.config(text="", font=("Arial", 8), wraplength=150)
            for var in self.check_vars:
                var.set(False)
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        self.scale_factor = 1.0  # 이미지 크기 배율
        self.base_width = 160  # 기본 캔버스 너비
        self.base_height = 60  # 기본 캔버스 높이

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        # ROI 선택 프레임
        self.roi_frame = tk.Frame(root)
        self.roi_frame.pack(pady=5)

        self.btn_roi1 = tk.Button(self.roi_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.roi_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.roi_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.roi_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 3x4 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        self.grid_checkbuttons = []
        for i in range(4):  # 4행
            for j in range(3):  # 3열
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=5, pady=5)
                canvas = tk.Canvas(frame, width=self.base_width, height=self.base_height, bg="gray")
                canvas.pack()
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                checkbox = tk.Checkbutton(frame, text="Select Image", variable=self.check_vars[i * 3 + j])
                checkbox.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)
                self.grid_checkbuttons.append(checkbox)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        # 버튼 및 상태 프레임
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_in = tk.Button(self.button_frame, text="+", command=self.zoom_in, state=tk.DISABLED)
        self.btn_zoom_in.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_out = tk.Button(self.button_frame, text="−", command=self.zoom_out, state=tk.DISABLED)
        self.btn_zoom_out.pack(side=tk.LEFT, padx=5)

        self.image_label = None
        self.grid_images = []
        self.roi_combined_size = None

    def select_folder(self):
        logging.info("Selecting folder")
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.scale_factor = 1.0
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.btn_zoom_in.config(state=tk.DISABLED)
                self.btn_zoom_out.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_filename.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)
        self.label_filename.config(text=f"Image: {os.path.basename(image_path)}")

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        img = cv2.imread(self.image_files[0])
        if img is None:
            logging.error("Failed to load first image for ROI sizing")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

        # 연결된 이미지 크기
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            h, w = combined_img.shape[:2]
            max_width = 1000
            max_height = 600
            scale = min(max_width/w, max_height/h, 1.0)
            self.base_width = int(w * scale)
            self.base_height = int(h * scale)
            self.roi_combined_size = (self.base_width, self.base_height)
            logging.info(f"ROI combined size: {self.roi_combined_size}")
        except cv2.error as e:
            logging.error(f"Error calculating ROI size: {e}")
            self.roi_combined_size = (160, 60)

        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.btn_zoom_in.config(state=tk.NORMAL)
        self.btn_zoom_out.config(state=tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def zoom_in(self):
        if self.scale_factor < 4.0:
            self.scale_factor += 0.1
            logging.info(f"Zoom in: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def zoom_out(self):
        if self.scale_factor > 0.25:
            self.scale_factor -= 0.1
            logging.info(f"Zoom out: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def update_grid_size(self):
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        font_size = max(6, int(8 * self.scale_factor))
        wraplength = int(150 * self.scale_factor)

        for canvas in self.grid_canvases:
            canvas.config(width=new_width, height=new_height)
        for label in self.grid_labels:
            label.config(font=("Arial", font_size), wraplength=wraplength)

        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        for var in self.check_vars:
            var.set(False)
        for cb in self.grid_checkbuttons:
            cb.config(state=tk.NORMAL)

        self.grid_images = []
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (new_width, new_height))
                self.display_image(self.grid_canvases[i], combined_img)
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")
            self.grid_checkbuttons[i].config(state=tk.DISABLED)

        # 상태 업데이트
        remaining_pages = math.ceil((len(self.image_files) - start_idx) / self.images_per_page) - 1
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)} (남은 페이지: {remaining_pages})")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)
        self.label_filename.config(text="")

    def save_selected_images(self):
        logging.info("Saving selected images")
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in detected_folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in detected_folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")

    def prev_image(self):
        self.save_selected_images()
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        self.save_selected_images()
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.btn_zoom_in.config(state=tk.DISABLED)
            self.btn_zoom_out.config(state=tk.DISABLED)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            self.scale_factor = 1.0
            self.base_width = 160
            self.base_height = 60
            for canvas in self.grid_canvases:
                canvas.delete("all")
                canvas.config(width=160, height=60)
            for label in self.grid_labels:
                label.config(text="", font=("Arial", 8), wraplength=150)
            for cb in self.grid_checkbuttons:
                cb.config(state=tk.DISABLED)
            for var in self.check_vars:
                var.set(False)
            self.label_filename.config(text="")
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        self.scale_factor = 1.0  # 이미지 크기 배율
        self.base_width = 160  # 기본 캔버스 너비 (참조용)
        self.base_height = 60  # 기본 캔버스 높이 (참조용)

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        # ROI 선택 프레임
        self.roi_frame = tk.Frame(root)
        self.roi_frame.pack(pady=5)

        self.btn_roi1 = tk.Button(self.roi_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.roi_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.roi_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.roi_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 4x3 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        self.grid_checkbuttons = []
        for i in range(3):
            for j in range(4):
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=5, pady=5)
                canvas = tk.Canvas(frame, width=self.base_width, height=self.base_height, bg="gray")
                canvas.pack()
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                checkbox = tk.Checkbutton(frame, text="Select Image", variable=self.check_vars[i * 4 + j])
                checkbox.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)
                self.grid_checkbuttons.append(checkbox)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        # 버튼 및 상태 프레임
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_in = tk.Button(self.button_frame, text="+", command=self.zoom_in, state=tk.DISABLED)
        self.btn_zoom_in.pack(side=tk.LEFT, padx=5)

        self.btn_zoom_out = tk.Button(self.button_frame, text="−", command=self.zoom_out, state=tk.DISABLED)
        self.btn_zoom_out.pack(side=tk.LEFT, padx=5)

        self.image_label = None
        self.grid_images = []
        self.roi_combined_size = None  # ROI1+ROI2 실제 크기 저장

    def select_folder(self):
        logging.info("Selecting folder")
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.scale_factor = 1.0
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.btn_zoom_in.config(state=tk.DISABLED)
                self.btn_zoom_out.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_filename.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)
        self.label_filename.config(text=f"Image: {os.path.basename(image_path)}")

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        # 첫 이미지로 ROI 실제 크기 계산
        img = cv2.imread(self.image_files[0])
        if img is None:
            logging.error("Failed to load first image for ROI sizing")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 크기
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

        # 연결된 이미지 크기
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            h, w = combined_img.shape[:2]
            # 창 크기에 맞게 스케일링
            max_width = 1000  # 최대 너비 제한
            max_height = 600  # 최대 높이 제한
            scale = min(max_width/w, max_height/h, 1.0)
            self.base_width = int(w * scale)
            self.base_height = int(h * scale)
            self.roi_combined_size = (self.base_width, self.base_height)
            logging.info(f"ROI combined size: {self.roi_combined_size}")
        except cv2.error as e:
            logging.error(f"Error calculating ROI size: {e}")
            self.roi_combined_size = (160, 60)

        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.btn_zoom_in.config(state=tk.NORMAL)
        self.btn_zoom_out.config(state=tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def zoom_in(self):
        if self.scale_factor < 4.0:
            self.scale_factor += 0.1
            logging.info(f"Zoom in: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def zoom_out(self):
        if self.scale_factor > 0.25:
            self.scale_factor -= 0.1
            logging.info(f"Zoom out: scale_factor={self.scale_factor}")
            self.update_grid_size()

    def update_grid_size(self):
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        font_size = max(6, int(8 * self.scale_factor))
        wraplength = int(150 * self.scale_factor)

        for canvas in self.grid_canvases:
            canvas.config(width=new_width, height=new_height)
        for label in self.grid_labels:
            label.config(font=("Arial", font_size), wraplength=wraplength)

        # 이미지 다시 그리기
        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        for var in self.check_vars:
            var.set(False)
        for cb in self.grid_checkbuttons:
            cb.config(state=tk.NORMAL)

        self.grid_images = []
        new_width = int(self.base_width * self.scale_factor)
        new_height = int(self.base_height * self.scale_factor)
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (new_width, new_height))
                self.display_image(self.grid_canvases[i], combined_img)
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")
            self.grid_checkbuttons[i].config(state=tk.DISABLED)

        # 상태 업데이트
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)}")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)
        self.label_filename.config(text="")

    def save_selected_images(self):
        logging.info("Saving selected images")
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in detected_folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in detected_folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")

    def prev_image(self):
        self.save_selected_images()
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        self.save_selected_images()
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.btn_zoom_in.config(state=tk.DISABLED)
            self.btn_zoom_out.config(state=tk.DISABLED)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            self.scale_factor = 1.0
            self.base_width = 160
            self.base_height = 60
            for canvas in self.grid_canvases:
                canvas.delete("all")
                canvas.config(width=160, height=60)
            for label in self.grid_labels:
                label.config(text="", font=("Arial", 8), wraplength=150)
            for cb in self.grid_checkbuttons:
                cb.config(state=tk.DISABLED)
            for var in self.check_vars:
                var.set(False)
            self.label_filename.config(text="")
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class ImageSelectorApp:
    def __init__(self, root):
        logging.info("Initializing ImageSelectorApp")
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        # ROI 선택 프레임
        self.roi_frame = tk.Frame(root)
        self.roi_frame.pack(pady=5)

        self.btn_roi1 = tk.Button(self.roi_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.roi_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.roi_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.roi_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 4x3 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        self.grid_checkbuttons = []
        for i in range(3):
            for j in range(4):
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=5, pady=5)
                canvas = tk.Canvas(frame, width=160, height=60, bg="gray")
                canvas.pack()
                label = tk.Label(frame, text="", wraplength=150, font=("Arial", 8))
                label.pack()
                checkbox = tk.Checkbutton(frame, text="Select Image", variable=self.check_vars[i * 4 + j])
                checkbox.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)
                self.grid_checkbuttons.append(checkbox)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        # 버튼 및 상태 프레임
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.image_label = None
        self.grid_images = []

    def select_folder(self):
        logging.info("Selecting folder")
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            # detected_folder 생성
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            logging.info(f"Found {len(self.image_files)} images")
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                # 체크박스 변수 초기화
                for var in self.check_vars:
                    var.set(False)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def set_roi_mode(self, mode):
        logging.info(f"Setting ROI mode: {mode}")
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        logging.info(f"Showing image: {image_path}")
        img = cv2.imread(image_path)
        if img is None:
            logging.error(f"Failed to load image: {image_path}")
            self.label_filename.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)
        self.label_filename.config(text=f"Image: {os.path.basename(image_path)}")

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            logging.info("Starting ROI selection")
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            logging.info("Ending ROI selection")
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                logging.warning("ROI too small, ignoring")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        logging.info("Starting processing")
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def show_roi_images(self):
        logging.info(f"Showing ROI images for page {self.current_page}")
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        # 체크박스 상태 초기화
        for var in self.check_vars:
            var.set(False)
        for cb in self.grid_checkbuttons:
            cb.config(state=tk.NORMAL)

        self.grid_images = []
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                logging.error(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                logging.warning(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(
                    roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
                )
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(
                    roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
                )

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (160, 60))
                self.display_image(self.grid_canvases[i], combined_img)
                # 파일 이름 자르기
                base_name = os.path.basename(img_path)
                display_name = base_name[:20] + "..." if len(base_name) > 20 else base_name
                self.grid_labels[i].config(text=display_name)
                self.grid_images.append(img_path)
            except cv2.error as e:
                logging.error(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")
            self.grid_checkbuttons[i].config(state=tk.DISABLED)

        # 상태 업데이트
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)}")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)
        self.label_filename.config(text="")

    def save_selected_images(self):
        logging.info("Saving selected images")
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                dst_path = os.path.join(self.save_folder, base_name)
                try:
                    if os.path.exists(src_path):
                        if os.path.exists(dst_path):
                            logging.warning(f"File {base_name} already exists in detected_folder")
                            messagebox.showwarning("Warning", f"File {base_name} already exists in detected_folder!")
                            continue
                        shutil.copy(src_path, dst_path)
                        logging.info(f"Saved {base_name} to {self.save_folder}")
                    else:
                        logging.error(f"File {base_name} not found")
                        messagebox.showwarning("Warning", f"File {base_name} not found!")
                except Exception as e:
                    logging.error(f"Error saving {base_name}: {e}")
                    messagebox.showerror("Error", f"Failed to save {base_name}: {e}")

    def prev_image(self):
        self.save_selected_images()
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        self.save_selected_images()
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.grid_images = []
            for canvas in self.grid_canvases:
                canvas.delete("all")
            for label in self.grid_labels:
                label.config(text="")
            for cb in self.grid_checkbuttons:
                cb.config(state=tk.DISABLED)
            for var in self.check_vars:
                var.set(False)
            self.label_filename.config(text="")
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")
            logging.info("Reset application state")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob
import math

class ImageSelectorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("1200x800")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_page = 0
        self.images_per_page = 12
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None
        self.check_vars = []

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        # ROI 선택 프레임
        self.roi_frame = tk.Frame(root)
        self.roi_frame.pack(pady=5)

        self.btn_roi1 = tk.Button(self.roi_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.roi_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.roi_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.roi_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        # 첫 이미지용 캔버스
        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        # 4x3 그리드용 프레임
        self.grid_frame = tk.Frame(root)
        self.grid_frame.pack(pady=10)
        self.grid_canvases = []
        self.grid_labels = []
        self.grid_checkboxes = []
        for i in range(3):
            for j in range(4):
                frame = tk.Frame(self.grid_frame)
                frame.grid(row=i, column=j, padx=5, pady=5)
                canvas = tk.Canvas(frame, width=160, height=60, bg="gray")
                canvas.pack()
                label = tk.Label(frame, text="", wraplength=160)
                label.pack()
                check_var = tk.BooleanVar()
                checkbox = tk.Checkbutton(frame, text="Select Image", variable=check_var)
                checkbox.pack()
                self.grid_canvases.append(canvas)
                self.grid_labels.append(label)
                self.grid_checkboxes.append(check_var)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        # 버튼 및 상태 프레임
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.label_status = tk.Label(self.button_frame, text="")
        self.label_status.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.image_label = None
        self.grid_images = []

    def select_folder(self):
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            # detected_folder 생성
            self.save_folder = os.path.join(self.folder_path, "detected_folder")
            os.makedirs(self.save_folder, exist_ok=True)
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102*.jpg")))
            if self.image_files:
                self.current_page = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
                for var, cb in zip(self.check_vars, self.grid_checkboxes):
                    cb.config(variable=var)
                self.show_image(self.image_files[0])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
                self.label_status.config(text="")
                self.grid_frame.pack_forget()
                self.canvas.pack(pady=10)
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102*.jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def set_roi_mode(self, mode):
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        img = cv2.imread(image_path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)
        self.display_image(self.canvas, img)
        self.label_filename.config(text=f"Image: {os.path.basename(image_path)}")

    def display_image(self, canvas, img):
        img_pil = Image.fromarray(img)
        photo = ImageTk.PhotoImage(img_pil)
        if canvas == self.canvas:
            if self.image_label:
                self.canvas.delete(self.image_label)
            self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            self.image_label_photo = photo
        else:
            canvas.delete("all")
            canvas.create_image(0, 0, anchor=tk.NW, image=photo)
            canvas.image = photo

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_page == 0 and self.selected_roi:
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                print("ROI too small, ignoring.")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.canvas.pack_forget()
        self.grid_frame.pack(pady=10)
        self.show_roi_images()

    def show_roi_images(self):
        start_idx = self.current_page * self.images_per_page
        end_idx = min(start_idx + self.images_per_page, len(self.image_files))
        self.check_vars = [tk.BooleanVar() for _ in range(self.images_per_page)]
        for i, var in enumerate(self.check_vars):
            self.grid_checkboxes[i].config(variable=var)
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")
            self.grid_checkboxes[i].config(state=tk.NORMAL)

        self.grid_images = []
        for i, idx in enumerate(range(start_idx, end_idx)):
            img_path = self.image_files[idx]
            img = cv2.imread(img_path)
            if img is None:
                print(f"Failed to load image: {img_path}")
                self.grid_labels[i].config(text="Error: Failed to load")
                continue
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            h, w = img.shape[:2]

            # ROI1 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                print(f"Invalid ROI1 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI1")
                continue
            roi1_img = img[y1:y2, x1:x2]

            # ROI2 좌표
            x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
            x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
            y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
            if x2 <= x1 or y2 <= y1:
                print(f"Invalid ROI2 for {img_path}: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
                self.grid_labels[i].config(text="Error: Invalid ROI2")
                continue
            roi2_img = img[y1:y2, x1:x2]

            # 높이 맞추기
            h1, w1 = roi1_img.shape[:2]
            h2, w2 = roi2_img.shape[:2]
            max_h = max(h1, h2)
            if h1 < max_h:
                padding = max_h - h1
                roi1_img = cv2.copyMakeBorder(
                    roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
                )
            elif h2 < max_h:
                padding = max_h - h2
                roi2_img = cv2.copyMakeBorder(
                    roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
                )

            # 이미지 연결
            try:
                combined_img = cv2.hconcat([roi1_img, roi2_img])
                combined_img = cv2.resize(combined_img, (160, 60))
                self.display_image(self.grid_canvases[i], combined_img)
                self.grid_labels[i].config(text=os.path.basename(img_path))
                self.grid_images.append(img_path)
            except cv2.error as e:
                print(f"OpenCV Error for {img_path}: {e}")
                self.grid_labels[i].config(text="Error: Failed to combine")

        # 빈 셀 비활성화
        for i in range(len(self.grid_images), self.images_per_page):
            self.grid_canvases[i].delete("all")
            self.grid_labels[i].config(text="")
            self.grid_checkboxes[i].config(state=tk.DISABLED)

        # 상태 업데이트
        self.label_status.config(text=f"{start_idx + 1}/{len(self.image_files)}")
        self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if end_idx >= len(self.image_files) else tk.NORMAL)
        self.label_filename.config(text="")

    def prev_image(self):
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                # annotated_image__<timestamp>_[102*.jpg -> image__<timestamp>_[102*.jpg
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                if os.path.exists(src_path):
                    shutil.copy(src_path, self.save_folder)
                else:
                    messagebox.showwarning("Warning", f"File {base_name} not found!")
        if self.current_page > 0:
            self.current_page -= 1
            self.show_roi_images()

    def next_image(self):
        start_idx = self.current_page * self.images_per_page
        for i, img_path in enumerate(self.grid_images):
            if self.check_vars[i].get():
                base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
                src_path = os.path.join(self.folder_path, base_name)
                if os.path.exists(src_path):
                    shutil.copy(src_path, self.save_folder)
                else:
                    messagebox.showwarning("Warning", f"File {base_name} not found!")
        self.current_page += 1
        if self.current_page * self.images_per_page < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_page == 0 else tk.NORMAL)
            self.current_page = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.check_vars = []
            self.grid_images = []
            for canvas in self.grid_canvases:
                canvas.delete("all")
            for label in self.grid_labels:
                label.config(text="")
            for cb in self.grid_checkboxes:
                cb.config(state=tk.DISABLED)
            self.label_filename.config(text="")
            self.label_status.config(text="")
            self.grid_frame.pack_forget()
            self.canvas.pack(pady=10)
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob

class ImageSelectorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("800x600")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_index = 0
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0
        self.selected_roi = None  # 현재 선택된 ROI ("ROI1" or "ROI2")

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        self.btn_select_save_folder = tk.Button(root, text="Select Save Folder", command=self.select_save_folder)
        self.btn_select_save_folder.pack(pady=5)

        # ROI 선택 버튼 및 좌표 표시
        self.roi_frame = tk.Frame(root)
        self.roi_frame.pack(pady=5)

        self.btn_roi1 = tk.Button(self.roi_frame, text="Select ROI1", command=lambda: self.set_roi_mode("ROI1"), state=tk.DISABLED)
        self.btn_roi1.pack(side=tk.LEFT, padx=5)
        self.label_roi1 = tk.Label(self.roi_frame, text="ROI1: Not selected")
        self.label_roi1.pack(side=tk.LEFT, padx=5)

        self.btn_roi2 = tk.Button(self.roi_frame, text="Select ROI2", command=lambda: self.set_roi_mode("ROI2"), state=tk.DISABLED)
        self.btn_roi2.pack(side=tk.LEFT, padx=5)
        self.label_roi2 = tk.Label(self.roi_frame, text="ROI2: Not selected")
        self.label_roi2.pack(side=tk.LEFT, padx=5)

        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("<Button-1>", self.start_roi)
        self.canvas.bind("<B1-Motion>", self.update_roi)
        self.canvas.bind("<ButtonRelease-1>", self.end_roi)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.check_var = tk.BooleanVar()
        self.checkbox = tk.Checkbutton(root, text="Select Image", variable=self.check_var)
        self.checkbox.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        # 버튼 프레임
        self.button_frame = tk.Frame(root)
        self.button_frame.pack(pady=5)

        self.btn_prev = tk.Button(self.button_frame, text="Previous", command=self.prev_image, state=tk.DISABLED)
        self.btn_prev.pack(side=tk.LEFT, padx=5)

        self.btn_next = tk.Button(self.button_frame, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(side=tk.LEFT, padx=5)

        self.image_label = None

    def select_folder(self):
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "annotated_image__*_[102].jpg")))
            if self.image_files:
                self.current_index = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.selected_roi = None
                self.show_image(self.image_files[self.current_index])
                self.btn_start.config(state=tk.NORMAL)
                self.btn_roi1.config(state=tk.NORMAL)
                self.btn_roi2.config(state=tk.DISABLED)  # ROI1 먼저 선택
                self.btn_prev.config(state=tk.DISABLED)
                self.btn_next.config(state=tk.DISABLED)
                self.label_roi1.config(text="ROI1: Not selected")
                self.label_roi2.config(text="ROI2: Not selected")
            else:
                messagebox.showwarning("Warning", "No annotated_image__*_[102].jpg files found!")
                self.btn_start.config(state=tk.DISABLED)
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.DISABLED)

    def select_save_folder(self):
        self.save_folder = filedialog.askdirectory(title="Select Save Folder")
        if self.save_folder:
            messagebox.showinfo("Info", f"Save folder selected: {self.save_folder}")

    def set_roi_mode(self, mode):
        self.selected_roi = mode
        self.btn_roi1.config(relief=tk.SUNKEN if mode == "ROI1" else tk.RAISED)
        self.btn_roi2.config(relief=tk.SUNKEN if mode == "ROI2" else tk.RAISED)

    def show_image(self, image_path):
        img = cv2.imread(image_path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.canvas.config(width=640, height=480)  # 전체 이미지 크기로 캔버스 설정
        self.display_image(img)
        self.label_filename.config(text=os.path.basename(image_path))

    def display_image(self, img):
        img_pil = Image.fromarray(img)
        self.photo = ImageTk.PhotoImage(img_pil)
        if self.image_label:
            self.canvas.delete(self.image_label)
        self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo)

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_index == 0 and self.selected_roi:
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if x2 - x1 < 10 or y2 - y1 < 10:
                print("ROI too small, ignoring.")
                self.canvas.delete("roi")
                return
            if self.selected_roi == "ROI1":
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count = 1
                self.canvas.delete("roi1")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
                self.label_roi1.config(text=f"ROI1: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi1.config(state=tk.DISABLED)
                self.btn_roi2.config(state=tk.NORMAL)
                self.selected_roi = None
            elif self.selected_roi == "ROI2":
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count = 2
                self.canvas.delete("roi2")
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
                self.label_roi2.config(text=f"ROI2: ({x1}, {y1}, {x2}, {y2})")
                self.btn_roi2.config(state=tk.DISABLED)
                self.selected_roi = None
            self.canvas.delete("roi")

    def start_processing(self):
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        if not self.save_folder:
            messagebox.showwarning("Warning", "Please select a save folder!")
            return
        self.btn_start.config(state=tk.DISABLED)
        self.btn_roi1.config(state=tk.DISABLED)
        self.btn_roi2.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.btn_prev.config(state=tk.DISABLED if self.current_index == 0 else tk.NORMAL)
        self.show_roi_images()

    def show_roi_images(self):
        img_path = self.image_files[self.current_index]
        img = cv2.imread(img_path)
        if img is None:
            print(f"Failed to load image: {img_path}")
            self.label_filename.config(text="Error: Failed to load image")
            return
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]

        # ROI1 좌표
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        if x2 <= x1 or y2 <= y1:
            print(f"Invalid ROI1: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
            self.canvas.config(width=640, height=480)
            self.display_image(img)
            self.label_filename.config(text="Error: Invalid ROI1")
            return
        roi1_img = img[y1:y2, x1:x2]

        # ROI2 좌표
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
        y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
        if x2 <= x1 or y2 <= y1:
            print(f"Invalid ROI2: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
            self.canvas.config(width=640, height=480)
            self.display_image(img)
            self.label_filename.config(text="Error: Invalid ROI2")
            return
        roi2_img = img[y1:y2, x1:x2]

        # 높이 맞추기 (패딩 추가)
        h1, w1 = roi1_img.shape[:2]
        h2, w2 = roi2_img.shape[:2]
        max_h = max(h1, h2)
        if h1 < max_h:
            padding = max_h - h1
            roi1_img = cv2.copyMakeBorder(
                roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
            )
        elif h2 < max_h:
            padding = max_h - h2
            roi2_img = cv2.copyMakeBorder(
                roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
            )

        # 이미지 연결
        try:
            combined_img = cv2.hconcat([roi1_img, roi2_img])
            combined_img = cv2.resize(combined_img, (640, 240))
            self.canvas.config(width=640, height=240)  # 캔버스 크기 조정
            self.display_image(combined_img)
            self.label_filename.config(text=f"Image: {os.path.basename(img_path)}")
            self.check_var.set(False)
        except cv2.error as e:
            print(f"OpenCV Error: {e}")
            self.canvas.config(width=640, height=480)
            self.display_image(img)
            self.label_filename.config(text="Error: Failed to combine images")

        # 버튼 상태 업데이트
        self.btn_prev.config(state=tk.DISABLED if self.current_index == 0 else tk.NORMAL)
        self.btn_next.config(state=tk.DISABLED if self.current_index == len(self.image_files) - 1 else tk.NORMAL)

    def prev_image(self):
        if self.check_var.get():
            img_path = self.image_files[self.current_index]
            base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
            src_path = os.path.join(self.folder_path, base_name)
            if os.path.exists(src_path):
                shutil.copy(src_path, self.save_folder)
            else:
                messagebox.showwarning("Warning", f"File {base_name} not found!")
        if self.current_index > 0:
            self.current_index -= 1
            self.show_roi_images()

    def next_image(self):
        if self.check_var.get():
            img_path = self.image_files[self.current_index]
            base_name = os.path.basename(img_path).replace("annotated_image__", "image__")
            src_path = os.path.join(self.folder_path, base_name)
            if os.path.exists(src_path):
                shutil.copy(src_path, self.save_folder)
            else:
                messagebox.showwarning("Warning", f"File {base_name} not found!")
        self.current_index += 1
        if self.current_index < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.btn_prev.config(state=tk.DISABLED if self.current_index == 0 else tk.NORMAL)
            self.current_index = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.label_filename.config(text="")
            self.canvas.delete("all")
            self.canvas.config(width=640, height=480)
            self.btn_start.config(state=tk.DISABLED)
            self.btn_roi1.config(state=tk.DISABLED)
            self.btn_roi2.config(state=tk.DISABLED)
            self.label_roi1.config(text="ROI1: Not selected")
            self.label_roi2.config(text="ROI2: Not selected")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()
```

```python
def show_roi_images(self):
    img_path = self.image_files[self.current_index]
    img = cv2.imread(img_path)
    if img is None:
        print(f"Failed to load image: {img_path}")
        self.label_filename.config(text="Error: Failed to load image")
        return
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    h, w = img.shape[:2]

    # ROI1 좌표
    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
    if x2 <= x1 or y2 <= y1:
        print(f"Invalid ROI1: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
        self.display_image(img)
        self.label_filename.config(text="Error: Invalid ROI1")
        return
    roi1_img = img[y1:y2, x1:x2]

    # ROI2 좌표
    x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
    x1, x2 = max(0, min(x1, x2)), min(w, max(x1, x2))
    y1, y2 = max(0, min(y1, y2)), min(h, max(y1, y2))
    if x2 <= x1 or y2 <= y1:
        print(f"Invalid ROI2: x1={x1}, x2={x2}, y1={y1}, y2={y2}")
        self.display_image(img)
        self.label_filename.config(text="Error: Invalid ROI2")
        return
    roi2_img = img[y1:y2, x1:x2]

    # 높이 맞추기 (패딩 추가)
    h1, w1 = roi1_img.shape[:2]
    h2, w2 = roi2_img.shape[:2]
    max_h = max(h1, h2)

    if h1 < max_h:
        # ROI1에 패딩 추가
        padding = max_h - h1
        roi1_img = cv2.copyMakeBorder(
            roi1_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
        )
    elif h2 < max_h:
        # ROI2에 패딩 추가
        padding = max_h - h2
        roi2_img = cv2.copyMakeBorder(
            roi2_img, 0, padding, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0)
        )

    # 이미지 연결
    try:
        combined_img = cv2.hconcat([roi1_img, roi2_img])
        combined_img = cv2.resize(combined_img, (640, 240))
        self.display_image(combined_img)
        self.label_filename.config(text=os.path.basename(img_path))
        self.check_var.set(False)
    except cv2.error as e:
        print(f"OpenCV Error: {e}")
        self.display_image(img)
        self.label_filename.config(text="Error: Failed to combine images")
```
 

```python
import os
import tkinter as tk
from tkinter import filedialog, messagebox
import cv2
from PIL import Image, ImageTk
import shutil
import glob

class ImageSelectorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Selector App")
        self.root.geometry("800x600")

        # 변수 초기화
        self.folder_path = ""
        self.save_folder = ""
        self.image_files = []
        self.current_index = 0
        self.roi1 = None
        self.roi2 = None
        self.is_selecting_roi = False
        self.temp_roi = [0, 0, 0, 0]
        self.roi_count = 0

        # GUI 요소
        self.label_folder = tk.Label(root, text="No folder selected")
        self.label_folder.pack(pady=5)

        self.btn_select_folder = tk.Button(root, text="Select Folder", command=self.select_folder)
        self.btn_select_folder.pack(pady=5)

        self.btn_select_save_folder = tk.Button(root, text="Select Save Folder", command=self.select_save_folder)
        self.btn_select_save_folder.pack(pady=5)

        self.canvas = tk.Canvas(root, width=640, height=480, bg="gray")
        self.canvas.pack(pady=10)
        self.canvas.bind("", self.start_roi)
        self.canvas.bind("", self.update_roi)
        self.canvas.bind("", self.end_roi)

        self.label_filename = tk.Label(root, text="")
        self.label_filename.pack(pady=5)

        self.check_var = tk.BooleanVar()
        self.checkbox = tk.Checkbutton(root, text="Select Image", variable=self.check_var)
        self.checkbox.pack(pady=5)

        self.btn_start = tk.Button(root, text="Start", command=self.start_processing, state=tk.DISABLED)
        self.btn_start.pack(pady=5)

        self.btn_next = tk.Button(root, text="Next", command=self.next_image, state=tk.DISABLED)
        self.btn_next.pack(pady=5)

        self.image_label = None

    def select_folder(self):
        self.folder_path = filedialog.askdirectory(title="Select Image Folder")
        if self.folder_path:
            self.label_folder.config(text=f"Selected Folder: {self.folder_path}")
            self.image_files = sorted(glob.glob(os.path.join(self.folder_path, "*[102].jpg")))
            if self.image_files:
                self.current_index = 0
                self.roi1 = None
                self.roi2 = None
                self.roi_count = 0
                self.show_image(self.image_files[self.current_index])
                self.btn_start.config(state=tk.NORMAL)
            else:
                messagebox.showwarning("Warning", "No [102].jpg files found in the selected folder!")
                self.btn_start.config(state=tk.DISABLED)

    def select_save_folder(self):
        self.save_folder = filedialog.askdirectory(title="Select Save Folder")
        if self.save_folder:
            messagebox.showinfo("Info", f"Save folder selected: {self.save_folder}")

    def show_image(self, image_path):
        img = cv2.imread(image_path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = min(640/w, 480/h)
        img = cv2.resize(img, (int(w*scale), int(h*scale)))

        self.current_image = img
        self.scale = scale
        self.display_image(img)
        self.label_filename.config(text=os.path.basename(image_path))

    def display_image(self, img):
        img_pil = Image.fromarray(img)
        self.photo = ImageTk.PhotoImage(img_pil)
        if self.image_label:
            self.canvas.delete(self.image_label)
        self.image_label = self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo)

    def start_roi(self, event):
        if self.roi_count < 2 and self.current_index == 0:
            self.is_selecting_roi = True
            self.temp_roi[0] = event.x
            self.temp_roi[1] = event.y
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y

    def update_roi(self, event):
        if self.is_selecting_roi:
            self.temp_roi[2] = event.x
            self.temp_roi[3] = event.y
            self.canvas.delete("roi")
            self.canvas.create_rectangle(
                self.temp_roi[0], self.temp_roi[1], self.temp_roi[2], self.temp_roi[3],
                outline="red", width=2, tags="roi"
            )

    def end_roi(self, event):
        if self.is_selecting_roi:
            self.is_selecting_roi = False
            x1, y1, x2, y2 = self.temp_roi
            x1, x2 = min(x1, x2), max(x1, x2)
            y1, y2 = min(y1, y2), max(y1, y2)
            if self.roi_count == 0:
                self.roi1 = [x1, y1, x2, y2]
                self.roi_count += 1
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="blue", width=2, tags="roi1")
            elif self.roi_count == 1:
                self.roi2 = [x1, y1, x2, y2]
                self.roi_count += 1
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="green", width=2, tags="roi2")
            self.canvas.delete("roi")

    def start_processing(self):
        if not self.roi1 or not self.roi2:
            messagebox.showwarning("Warning", "Please select both ROIs!")
            return
        if not self.save_folder:
            messagebox.showwarning("Warning", "Please select a save folder!")
            return
        self.btn_start.config(state=tk.DISABLED)
        self.btn_next.config(state=tk.NORMAL)
        self.show_roi_images()

    def show_roi_images(self):
        img_path = self.image_files[self.current_index]
        img = cv2.imread(img_path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h, w = img.shape[:2]
        scale = self.scale
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi1]
        roi1_img = img[y1:y2, x1:x2]
        x1, y1, x2, y2 = [int(x/self.scale) for x in self.roi2]
        roi2_img = img[y1:y2, x1:x2]
        combined_img = cv2.hconcat([roi1_img, roi2_img])
        combined_img = cv2.resize(combined_img, (640, 240))
        self.display_image(combined_img)
        self.label_filename.config(text=os.path.basename(img_path))
        self.check_var.set(False)

    def next_image(self):
        if self.check_var.get():
            img_path = self.image_files[self.current_index]
            base_name = os.path.basename(img_path).replace("[102].jpg", ".jpg")
            src_path = os.path.join(self.folder_path, base_name)
            if os.path.exists(src_path):
                shutil.copy(src_path, self.save_folder)
            else:
                messagebox.showwarning("Warning", f"File {base_name} not found!")

        self.current_index += 1
        if self.current_index < len(self.image_files):
            self.show_roi_images()
        else:
            messagebox.showinfo("Info", "All images processed!")
            self.btn_next.config(state=tk.DISABLED)
            self.current_index = 0
            self.roi1 = None
            self.roi2 = None
            self.roi_count = 0
            self.image_files = []
            self.label_filename.config(text="")
            self.canvas.delete("all")
            self.btn_start.config(state=tk.DISABLED)

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSelectorApp(root)
    root.mainloop()

```

동작 설명
	1	폴더 선택: “Select Folder” 버튼을 눌러 [102].jpg 파일이 있는 폴더를 선택합니다. 첫 번째 이미지가 캔버스에 표시됩니다.
	2	ROI 선택: 첫 이미지에서 마우스로 두 개의 ROI를 드래그하여 선택합니다. 첫 번째 ROI는 파란색, 두 번째 ROI는 초록색으로 표시됩니다.
	3	저장 폴더 선택: “Select Save Folder” 버튼으로 저장할 폴더를 선택합니다.
	4	처리 시작: “Start” 버튼을 누르면 ROI 영역만 잘라서 두 개의 ROI를 가로로 붙여 화면에 표시합니다. 파일 이름과 체크박스도 함께 표시됩니다.
	5	이미지 선택 및 저장: 체크박스를 선택하고 “Next” 버튼을 누르면 해당 [102].jpg 파일과 이름이 같은 .jpg 파일이 지정된 폴더에 저장됩니다.
	6	반복: 모든 이미지를 처리할 때까지 다음 이미지를 표시하며 반복합니다.
추가 참고
	•	이미지 크기 조정: 이미지는 캔버스 크기(640x480)에 맞게 조정됩니다.
	•	ROI 선택 제한: ROI는 첫 번째 이미지에서만 선택 가능하며, 이후에는 고정된 ROI를 사용합니다.
	•	오류 처리: 폴더에 [102].jpg 파일이 없거나 저장 폴더가 선택되지 않은 경우 경고 메시지가 표시됩니다.
	•	Ubuntu 환경: Tkinter와 OpenCV는 Ubuntu에서 잘 동작하며, 별도의 GUI 설정이 필요 없습니다.
필요한 경우 코드 수정이나 추가 설명을 요청해주세요!
