현재 코드에서 `Tkinter` GUI와 `rclpy` ROS2 노드가 동시에 동작하지 않는 이유는 `rclpy.spin()`이 실행되면 해당 노드가 이벤트 처리를 계속해서 대기하는 상태가 되기 때문에, GUI가 실행되지 않거나 정지된 것처럼 보일 수 있습니다.

이를 해결하기 위해 `rclpy` 노드와 GUI를 각각 별도의 스레드 또는 프로세스에서 실행할 필요가 있습니다. 또한 `multiprocessing.Process` 대신 `threading`을 사용하는 것이 더 적합할 수 있습니다. ROS2는 멀티프로세싱에 민감할 수 있기 때문에, 멀티스레드를 사용하는 것이 더 안전합니다.

아래는 스레드를 사용한 리팩토링된 코드입니다:

### 리팩토링 코드:

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from taskmanager_message.msg import MTtopic   # Mass to Task Manager topic
from taskmanager_message.msg import TMtopic   # Task Manager to Mass topic
from taskmanager_message.srv import MTservice # Mass to Task Manager service
from taskmanager_message.srv import MassToTaskManagerBartControl as MTBartControl # Mass to Task Manager service (this is server)
from taskmanager_message.srv import TMservice # Task Manager to Mass service

import tkinter as tk
from tkinter import messagebox
import threading

topic_pub_cnt = 0

# ROS2 노드 정의
class MassDummy(Node):
    def __init__(self):
        super().__init__('mass_dummy')
        self.publisher_ = self.create_publisher(MTtopic, 'MTtopic', 10)
        self.timer = self.create_timer(1.0, self.publish_data_MT)
        self.subscription = self.create_subscription(TMtopic, 'TMtopic', self.listener_callback_tm, 10)

        self.srv_client = self.create_client(MTservice, 'MTservice')  
        self.srv_client_mt_bartcontrol = self.create_client(MTBartControl, 'MTBartControl')  

        self.srv_server = self.create_service(TMservice, 'TMservice', self.handle_tm_service_request)

    # topic publisher
    def publish_data_MT(self):
        global topic_pub_cnt
        msg = MTtopic()
        msg.behavior_planer_status = 1
        
        topic_pub_cnt += msg.behavior_planer_status

        self.publisher_.publish(msg)
        print("-----------------------------------------------")
        print("[Topic][publish] MTtopic  :" , msg.behavior_planer_status, ", pub_cnt : " , topic_pub_cnt)

        if topic_pub_cnt % 2 == 0:
            self.call_srv_status(topic_pub_cnt)
            self.call_srv_bart(topic_pub_cnt)

    # topic subscriber
    def listener_callback_tm(self, msg):
        print("-----------------------------------------------")
        print(f"[Topic][subcribe] TMtopic : {msg.bart_degree}")

    # Mass to Task Manager service call 
    def call_srv_status(self, data):
        request = MTservice.Request()
        request.service_finish_status = 1
        request.left_dish_status = 1
        request.right_dish_status = 1

        self.srv_client.call_async(request).add_done_callback(self.handle_mt_service_response)
        print("[service][request][MASS] -> [TASKmanager]")

    def handle_mt_service_response(self, future):
        response = future.result()
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt)

    # Mass to Task Manager service call 
    def call_srv_bart(self, data):
        request = MTBartControl.Request()
        request.bart_control = 1

        self.srv_client_mt_bartcontrol.call_async(request).add_done_callback(self.handle_mt_bart_service_response)
        print("[service][request][MASS] -> [TASKmanager]")

    def handle_mt_bart_service_response(self, future):
        response = future.result()
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt_bart_control)

    # Task Manager to Mass service call (server)
    def handle_tm_service_request(self, request, response):
        print(f"[service][receive][Mass] <- [TaskManager] : {request.robot_control_cmd}")
        response.response_tm = 10
        return response

# GUI 클래스 정의
class GUIControl:
    def __init__(self, ros_node):
        self.ros_node = ros_node

        self.root = tk.Tk()
        self.root.title("MASS TEST GUI")
        self.root.geometry("300x200")

        # 메뉴 생성
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        filemenu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="파일", menu=filemenu)
        filemenu.add_command(label="종료", command=self.root.quit)

        # 버튼 추가
        button = tk.Button(self.root, text="메시지", command=self.show_message)
        button.pack(pady=5)

        button1 = tk.Button(self.root, text="call_srv_status", command=self.call_srv_status)
        button1.pack(pady=5)

        button2 = tk.Button(self.root, text="call_srv_bart", command=self.call_srv_bart)
        button2.pack(pady=5)

        # 텍스트 레이블 추가
        label = tk.Label(self.root, text="MASS TEST GUI입니다.")
        label.pack(pady=10)

    def show_message(self):
        messagebox.showinfo("메시지", "버튼이 클릭되었습니다!")

    def call_srv_status(self):
        self.ros_node.call_srv_status(1)

    def call_srv_bart(self):
        self.ros_node.call_srv_bart(1)

    def start(self):
        self.root.mainloop()

def ros_spin(node):
    rclpy.spin(node)

def main(args=None):
    rclpy.init(args=args)

    # ROS2 노드 생성
    ros_node = MassDummy()

    # 스레드를 이용해 ROS2 노드와 GUI 동시 실행
    ros_thread = threading.Thread(target=ros_spin, args=(ros_node,))
    ros_thread.start()

    # GUI 시작
    gui = GUIControl(ros_node)
    gui.start()

    # ROS 종료 처리
    ros_thread.join()
    ros_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
```

### 변경 사항:
1. **스레드 사용**: `threading`을 이용하여 `rclpy` 노드와 GUI를 각각 다른 스레드에서 실행하도록 했습니다. `rclpy.spin()`은 별도의 스레드에서 동작하고, GUI는 메인 스레드에서 실행됩니다.
   
2. **GUI 및 ROS 노드 통신**: `GUIControl` 클래스는 `MassDummy` ROS2 노드를 참조로 받아, 버튼 클릭 시 ROS 서비스 호출이 가능하게 했습니다.

3. **동시 실행**: `ros_spin()` 함수는 ROS2 노드를 스핀하도록 하고, `main()` 함수에서 이를 스레드로 실행하여 GUI와 동시에 동작하게 했습니다.

이제 GUI와 ROS 노드가 동시에 동작할 것이며, GUI 버튼을 클릭하면 ROS 서비스가 호출됩니다.
