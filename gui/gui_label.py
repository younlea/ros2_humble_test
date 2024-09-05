`Tkinter`에서 버튼이나 텍스트 레이블 등을 배치할 때, 버튼 옆에 다른 요소를 배치하려면 `pack()` 대신 `grid()` 레이아웃을 사용하는 것이 더 유용합니다. `grid()`를 사용하면 행과 열을 지정하여 GUI 요소를 원하는 위치에 배치할 수 있습니다.

아래는 `button2` 옆에 `current status`를 표시하는 `Label`을 추가하는 수정된 `GUIControl` 클래스 부분입니다.

### 수정된 GUI 부분 코드:

```python
class GUIControl:
    def __init__(self, ros_node):
        self.ros_node = ros_node

        self.root = tk.Tk()
        self.root.title("MASS TEST GUI")
        self.root.geometry("400x200")  # 창 크기 조절

        # 메뉴 생성
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        filemenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="파일", menu=filemenu)
        filemenu.add_command(label="종료", command=self.root.quit)

        # 버튼 및 레이블 배치
        self.label_status = tk.Label(self.root, text="Current Status: N/A", width=20)  # 현재 상태를 보여줄 레이블
        button1 = tk.Button(self.root, text="call_srv_status", command=self.call_srv_status)
        button2 = tk.Button(self.root, text="call_srv_bart", command=self.call_srv_bart)

        # 그리드를 사용해 버튼과 레이블 배치
        button1.grid(row=0, column=0, padx=10, pady=10)
        button2.grid(row=0, column=1, padx=10, pady=10)
        self.label_status.grid(row=0, column=2, padx=10, pady=10)

        # 텍스트 레이블 추가 (위에 위치)
        label = tk.Label(self.root, text="MASS TEST GUI입니다.")
        label.grid(row=1, column=0, columnspan=3, pady=10)

    def show_message(self):
        messagebox.showinfo("메시지", "버튼이 클릭되었습니다!")

    def call_srv_status(self):
        # 서비스 호출 후 상태 업데이트
        self.ros_node.call_srv_status(1)
        self.update_status("Status: Service Called")

    def call_srv_bart(self):
        # 서비스 호출 후 상태 업데이트
        self.ros_node.call_srv_bart(1)
        self.update_status("Bart Control Called")

    def update_status(self, status):
        # 상태 레이블 업데이트
        self.label_status.config(text=status)

    def start(self):
        self.root.mainloop()
```

### 설명:
1. **`grid()` 레이아웃 사용**: 
   - `button1`과 `button2`를 각각 `grid()`의 `row=0`에 배치하고, 그 옆에 `self.label_status`를 추가했습니다. `column=0`, `column=1`, `column=2`로 각각 배치하여 버튼 두 개와 상태 표시 레이블이 한 줄에 위치하게 했습니다.

2. **상태 레이블 (`label_status`)**: 
   - `Label` 위젯을 사용해 버튼 옆에 "Current Status"를 표시하고, 버튼 클릭 시 상태를 업데이트하도록 했습니다. `call_srv_status()`와 `call_srv_bart()`가 실행될 때마다 `update_status()`를 호출하여 레이블의 텍스트를 변경합니다.

3. **GUI 크기 조절**: 
   - GUI 창의 크기를 400x200으로 변경하여 버튼과 레이블이 모두 잘 보이게 했습니다.

이제 `button2` 옆에 "Current Status"라는 레이블이 나타나고, 버튼을 클릭하면 해당 상태가 업데이트됩니다.
