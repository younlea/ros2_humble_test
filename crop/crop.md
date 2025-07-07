
```python
import tkinter as tk
from tkinter import filedialog, messagebox
import os
import cv2
import numpy as np
from PIL import Image, ImageTk
import glob

class ImageCropApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Cropper")
        self.folder_path = ""
        self.image_list = []
        self.current_image = None
        self.roi = None
        self.start_x, self.start_y = -1, -1
        self.rect_id = None

        # GUI 구성
        self.canvas = tk.Canvas(root, width=800, height=600)
        self.canvas.pack(pady=10)

        btn_frame = tk.Frame(root)
        btn_frame.pack(pady=5)

        tk.Button(btn_frame, text="폴더 선택", command=self.select_folder).pack(side=tk.LEFT, padx=5)
        tk.Button(btn_frame, text="시작", command=self.process_images).pack(side=tk.LEFT, padx=5)

        # 마우스 이벤트 바인딩
        self.canvas.bind("<ButtonPress-1>", self.start_rect)
        self.canvas.bind("<B1-Motion>", self.update_rect)
        self.canvas.bind("<ButtonRelease-1>", self.end_rect)

    def select_folder(self):
        self.folder_path = filedialog.askdirectory()
        if self.folder_path:
            # JPG 파일 목록 가져오기
            self.image_list = sorted(glob.glob(os.path.join(self.folder_path, "*.jpg")))
            if not self.image_list:
                messagebox.showerror("오류", "선택한 폴더에 JPG 이미지가 없습니다.")
                return
            # 첫 번째 이미지 표시
            self.display_image(self.image_list[0])

    def display_image(self, image_path):
        # 이미지 로드 및 표시
        img = cv2.imread(image_path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        self.current_image = img

        # 캔버스 크기에 맞게 리사이즈
        img_pil = Image.fromarray(img)
        img_pil.thumbnail((800, 600), Image.Resampling.LANCZOS)
        self.photo = ImageTk.PhotoImage(img_pil)
        self.canvas.config(width=self.photo.width(), height=self.photo.height())
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.photo)

    def start_rect(self, event):
        self.start_x, self.start_y = event.x, event.y
        if self.rect_id:
            self.canvas.delete(self.rect_id)

    def update_rect(self, event):
        if self.rect_id:
            self.canvas.delete(self.rect_id)
        self.rect_id = self.canvas.create_rectangle(
            self.start_x, self.start_y, event.x, event.y, outline="red", width=2
        )

    def end_rect(self, event):
        end_x, end_y = event.x, event.y
        # 원본 이미지 크기로 ROI 좌표 변환
        img_h, img_w = self.current_image.shape[:2]
        canvas_w, canvas_h = self.photo.width(), self.photo.height()
        scale_x, scale_y = img_w / canvas_w, img_h / canvas_h

        x1 = int(min(self.start_x, end_x) * scale_x)
        y1 = int(min(self.start_y, end_y) * scale_y)
        x2 = int(max(self.start_x, end_x) * scale_x)
        y2 = int(max(self.start_y, end_y) * scale_y)

        self.roi = (x1, y1, x2, y2)
        # ROI 영역 미리보기
        roi_img = self.current_image[y1:y2, x1:x2]
        roi_pil = Image.fromarray(roi_img)
        roi_pil.thumbnail((200, 200), Image.Resampling.LANCZOS)
        roi_photo = ImageTk.PhotoImage(roi_pil)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=roi_photo)
        self.canvas.image = roi_photo  # 참조 유지

    def process_images(self):
        if not self.image_list:
            messagebox.showerror("오류", "먼저 폴더를 선택하세요.")
            return
        if not self.roi:
            messagebox.showerror("오류", "ROI 영역을 선택하세요.")
            return

        x1, y1, x2, y2 = self.roi
        output_dir = os.path.join(self.folder_path, "cropped")
        os.makedirs(output_dir, exist_ok=True)

        # 이미지 쌍 처리
        for i in range(0, len(self.image_list), 2):
            # 첫 번째 이미지 크롭
            img1 = cv2.imread(self.image_list[i])
            crop1 = img1[y1:y2, x1:x2]
            crop_height, crop_width = crop1.shape[:2]

            # 두 번째 이미지가 있으면 크롭, 없으면 단일 이미지 처리
            if i + 1 < len(self.image_list):
                img2 = cv2.imread(self.image_list[i + 1])
                crop2 = img2[y1:y2, x1:x2]
                # 두 이미지를 수직으로 결합
                combined = np.vstack((crop1, crop2))
            else:
                # 홀수일 경우 단일 이미지
                combined = crop1

            # 저장
            output_path = os.path.join(output_dir, f"[crop]_{i//2 + 1}.jpg")
            cv2.imwrite(output_path, combined)

        messagebox.showinfo("완료", f"크롭된 이미지가 {output_dir}에 저장되었습니다.")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageCropApp(root)
    root.mainloop()
```
