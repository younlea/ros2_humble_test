아래는 요청하신 대로 Python과 Tkinter를 사용해 Ubuntu에서 동작하는 GUI 애플리케이션의 예제 코드입니다. 이 프로그램은 특정 폴더에서 [102].jpg 파일을 읽고, 사용자가 ROI(Region of Interest)를 선택한 후, 이미지를 순차적으로 보여주며 체크된 파일을 지정된 폴더에 저장합니다. OpenCV와 PIL을 사용해 이미지 처리를 수행합니다.
요구 사항
	•	Python 3.x
	•	필요한 라이브러리: opencv-python, Pillow, tkinter pip install opencv-python Pillow
	•	
코드
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
