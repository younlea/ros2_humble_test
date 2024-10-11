ROS에서 파이썬으로 UI를 만들기 위해서는 일반적으로 `Tkinter`, `PyQt`, 또는 `PySide` 같은 GUI 라이브러리를 사용할 수 있습니다. 여기서는 `Tkinter`를 사용하여 요구사항에 맞는 간단한 UI를 구현하는 방법을 설명하겠습니다. 이 프로그램은 시작 및 종료 시간을 기록하고, 성공 또는 실패 여부를 선택할 수 있으며, 결과를 CSV 파일로 저장하는 기능을 포함합니다.

```python
import tkinter as tk
from tkinter import ttk
from datetime import datetime
import csv

class Application(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ROS Python UI")
        self.geometry("600x400")

        self.records = []
        self.create_widgets()

    def create_widgets(self):
        # Left panel with start and stop buttons
        left_frame = tk.Frame(self)
        left_frame.pack(side=tk.LEFT, fill=tk.Y)

        self.start_button = tk.Button(left_frame, text="Start", command=self.start_record)
        self.start_button.pack(pady=10)

        self.stop_button = tk.Button(left_frame, text="Stop", command=self.stop_record)
        self.stop_button.pack(pady=10)

        # Right panel with records display
        right_frame = tk.Frame(self)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(right_frame)
        self.scrollbar = ttk.Scrollbar(right_frame, orient="vertical", command=self.canvas.yview)
        
        self.scrollable_frame = ttk.Frame(self.canvas)
        
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )

        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")

        self.canvas.configure(yscrollcommand=self.scrollbar.set)

        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")

    def start_record(self):
        start_time = datetime.now()
        record = {
            "start_time": start_time,
            "end_time": None,
            "duration": None,
            "status": None
        }
        
        frame = ttk.Frame(self.scrollable_frame)
        
        start_label = ttk.Label(frame, text=start_time.strftime("%Y-%m-%d %H:%M:%S"))
        start_label.pack(side=tk.LEFT)

        end_label = ttk.Label(frame, text="")
        end_label.pack(side=tk.LEFT)

        duration_label = ttk.Label(frame, text="")
        duration_label.pack(side=tk.LEFT)

        success_button = tk.Button(frame, text="Success", command=lambda: self.set_status(record, "Success"))
        success_button.pack(side=tk.LEFT)

        failure_button = tk.Button(frame, text="Failure", command=lambda: self.set_status(record, "Failure"))
        failure_button.pack(side=tk.LEFT)

        frame.pack(fill=tk.X)
        
        record["frame"] = frame
        record["end_label"] = end_label
        record["duration_label"] = duration_label

        if len(self.records) < 30:
            self.records.append(record)

    def stop_record(self):
        for record in self.records:
            if record["end_time"] is None:
                end_time = datetime.now()
                record["end_time"] = end_time
                record["end_label"].config(text=end_time.strftime("%Y-%m-%d %H:%M:%S"))

                duration = end_time - record["start_time"]
                record["duration"] = duration
                record["duration_label"].config(text=str(duration))

                # Save to CSV
                with open('records.csv', 'w', newline='') as csvfile:
                    fieldnames = ['Start Time', 'End Time', 'Duration', 'Status']
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

                    writer.writeheader()
                    for rec in self.records:
                        writer.writerow({
                            'Start Time': rec['start_time'].strftime("%Y-%m-%d %H:%M:%S"),
                            'End Time': rec['end_time'].strftime("%Y-%m-%d %H:%M:%S") if rec['end_time'] else '',
                            'Duration': str(rec['duration']) if rec['duration'] else '',
                            'Status': rec['status'] if rec['status'] else ''
                        })

    def set_status(self, record, status):
        record["status"] = status

if __name__ == "__main__":
    app = Application()
    app.mainloop()
```

### 주요 기능 설명:

- **시작 및 종료 시간**: 시작 버튼을 누르면 현재 시간이 기록되고 새로운 레코드가 생성됩니다. 종료 버튼을 누르면 모든 레코드의 종료 시간이 기록됩니다.
- **성공/실패 버튼**: 각 레코드에 대해 성공 또는 실패 상태를 설정할 수 있습니다.
- **CSV 파일 저장**: 종료 버튼을 누르면 모든 레코드가 CSV 파일에 저장됩니다.
- **스크롤 기능**: 많은 레코드가 생성될 경우 스크롤바를 통해 쉽게 탐색할 수 있습니다.

이 코드는 기본적인 GUI 기능을 제공하며 필요에 따라 확장할 수 있습니다.

출처
