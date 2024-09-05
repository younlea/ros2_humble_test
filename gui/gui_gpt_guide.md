이를 구현하기 위해, `ROS` 쪽 데이터를 반영하여 `Current Status` 레이블을 업데이트하고, 두 개의 `Text` 창에서 입력된 값을 읽어 특정 동작(예: ROS 서비스 호출)을 할 수 있게끔 구현해야 합니다.

### 주요 작업:
1. **`Current Status` 업데이트**: ROS 쪽 데이터를 읽어서 상태 레이블에 표시.
2. **텍스트 창 추가**: 두 개의 `Text` 창을 추가하고, 버튼을 누르면 입력된 값을 읽어 로직을 실행.
3. **버튼을 눌렀을 때 입력된 값 사용**: 버튼3을 누르면 `Text` 창에 입력된 숫자를 읽어와서 ROS 서비스 호출에 활용.

아래는 이 요구 사항을 반영한 코드입니다.

### 수정된 GUI 부분 코드:

```python
class GUIControl:
    def __init__(self, ros_node):
        self.ros_node = ros_node

        self.root = tk.Tk()
        self.root.title("MASS TEST GUI")
        self.root.geometry("500x300")  # 창 크기 조절

        # 메뉴 생성
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        filemenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="파일", menu=filemenu)
        filemenu.add_command(label="종료", command=self.root.quit)

        # 버튼 및 레이블 배치
        self.label_status = tk.Label(self.root, text="Current Status: N/A", width=30)  # 현재 상태를 보여줄 레이블
        button1 = tk.Button(self.root, text="call_srv_status", command=self.call_srv_status)
        button2 = tk.Button(self.root, text="call_srv_bart", command=self.call_srv_bart)

        # 텍스트 창 두 개 추가
        self.text_entry_1 = tk.Text(self.root, height=1, width=20)
        self.text_entry_2 = tk.Text(self.root, height=1, width=20)
        
        # 버튼3 추가 (텍스트 창에서 숫자 값을 읽음)
        button3 = tk.Button(self.root, text="Send Values", command=self.send_text_values)

        # 그리드를 사용해 버튼과 레이블, 텍스트 창 배치
        button1.grid(row=0, column=0, padx=10, pady=10)
        button2.grid(row=0, column=1, padx=10, pady=10)
        self.label_status.grid(row=0, column=2, padx=10, pady=10)

        # 텍스트 창 배치
        self.text_entry_1.grid(row=1, column=0, padx=10, pady=10)
        self.text_entry_2.grid(row=1, column=1, padx=10, pady=10)
        button3.grid(row=1, column=2, padx=10, pady=10)

        # 텍스트 레이블 추가 (위에 위치)
        label = tk.Label(self.root, text="MASS TEST GUI입니다.")
        label.grid(row=2, column=0, columnspan=3, pady=10)

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
        # 상태 레이블 업데이트 (ROS 변수를 반영)
        current_status = self.ros_node.get_current_status()  # ROS 노드의 상태를 가져오는 함수
        self.label_status.config(text=f"Current Status: {current_status}")

    def send_text_values(self):
        # 텍스트 창에 입력된 값 가져오기
        try:
            value1 = int(self.text_entry_1.get("1.0", "end-1c"))  # 첫 번째 Text 창의 값
            value2 = int(self.text_entry_2.get("1.0", "end-1c"))  # 두 번째 Text 창의 값

            # 입력된 값을 ROS 노드의 로직에 반영
            self.ros_node.process_values(value1, value2)
            self.update_status(f"Values Sent: {value1}, {value2}")

        except ValueError:
            messagebox.showerror("Error", "숫자를 입력하세요!")

    def start(self):
        self.root.mainloop()

```

### ROS 노드에서 사용될 메서드들:

#### `get_current_status()`:
이 함수는 ROS 노드의 상태를 반환하는 간단한 메서드입니다. `GUI`에서 상태를 확인할 때 호출됩니다.

```python
class MassDummy(Node):
    def __init__(self):
        super().__init__('mass_dummy')
        self.current_status = "Idle"  # 초기 상태

    def get_current_status(self):
        # 현재 노드의 상태를 반환
        return self.current_status

    # 이 메서드는 예시일 뿐입니다. 실제로는 노드 내부 상태 변화를 반영하도록 구현하세요.
    def update_current_status(self, new_status):
        self.current_status = new_status
```

#### `process_values(value1, value2)`:
이 함수는 GUI에서 입력된 값을 처리하는 역할을 합니다. 예를 들어, 입력된 값에 따라 ROS 서비스나 토픽을 호출할 수 있습니다.

```python
class MassDummy(Node):
    def __init__(self):
        super().__init__('mass_dummy')

    def process_values(self, value1, value2):
        # 입력된 값(value1, value2)을 사용하여 서비스 호출 또는 토픽 발행 등의 동작 수행
        print(f"Processing values: {value1}, {value2}")
        # 여기에서 ROS2의 로직을 추가합니다. 예: 서비스 호출
        self.call_srv_status(value1)
        self.call_srv_bart(value2)
```

### 코드 설명:
1. **텍스트 창 추가**: `self.text_entry_1`과 `self.text_entry_2` 두 개의 `Text` 창을 추가하여 숫자를 입력할 수 있도록 했습니다.
   
2. **버튼3 클릭 이벤트**: `send_text_values()` 메서드를 통해 버튼3 클릭 시, 두 개의 텍스트 창에서 값을 읽어와 숫자로 변환한 뒤, `process_values()` 메서드를 호출하여 ROS2 노드에서 처리할 수 있게 했습니다.

3. **상태 업데이트**: `update_status()` 메서드는 `ros_node`의 상태를 반영하여 `label_status`에 표시합니다. 여기서 `get_current_status()` 메서드를 호출해 ROS 노드의 상태를 받아옵니다.

4. **예외 처리**: 숫자가 아닌 값을 입력했을 때 발생하는 예외를 처리하여, 숫자가 아닌 값을 입력했을 경우 `messagebox`로 오류 메시지를 표시합니다.

이제 `button2` 옆에는 ROS 노드의 상태가 표시되고, 텍스트 창 두 개에 숫자를 입력한 뒤 버튼3을 누르면 해당 숫자가 ROS 노드의 처리 로직으로 전달됩니다.
