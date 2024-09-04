import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from taskmanager_message.msg import MTtopic   # Mass to Task Manager topic
from taskmanager_message.msg import TMtopic   # Task Manager to Mass topic
from taskmanager_message.srv import MTservice # Mass to Task Manager service
from taskmanager_message.srv import MassToTaskManagerBartControl as MTBartControl # Mass to Task Manager service (this is server)

from taskmanager_message.srv import TMservice # Task Manager to Mass service

topic_pub_cnt = 0


class mass_dummy(Node):
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
        msg = MTtopic()
        msg.behavior_planer_status = 1
        
        global topic_pub_cnt
        topic_pub_cnt= topic_pub_cnt + msg.behavior_planer_status

        self.publisher_.publish(msg)
        print("-----------------------------------------------")
        print("[Topic][publish] MTtopic  :" , msg.behavior_planer_status, ", pub_cnt : " , topic_pub_cnt)

        # temp code for service call test
        # will be remove after implement real logic in mass dummy node.
        if(topic_pub_cnt % 2 == 0):
            self.call_srv_status(topic_pub_cnt)
            self.call_srv_bart(topic_pub_cnt)
            

 # topic sbuscriber
    def listener_callback_tm(self, msg):
        print("-----------------------------------------------")
        print(f"[Topic][subcribe] TMtopic : {msg.bart_degree}")

    # Mass to Task Manager service call 
    def call_srv_status(self, data):
        request = MTservice.Request()
        # will be change below params using data value. 
        request.service_finish_status = 1
        request.left_dish_status = 1
        request.right_dish_status = 1

        self.srv_client.call_async(request).add_done_callback(self.handle_mt_service_response)
        print("---------------------------" )
        print("[service][request][MASS] -> [TASKmanager]" )
        print("service_finish_status", request.service_finish_status)
        print("left_dish_status", request.left_dish_status)
        print("right_dish_status", request.right_dish_status)

    def handle_mt_service_response(self, future):
        response = future.result()
        print("---------------------------" )
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt)

    # Mass to Task Manager service call 
    def call_srv_bart(self, data):
        request = MTBartControl.Request()
        # will be change below params using data value. 
        request.bart_control = 1

        self.srv_client_mt_bartcontrol.call_async(request).add_done_callback(self.handle_mt_bart_service_response)
        print("---------------------------" )
        print("[service][request][MASS] -> [TASKmanager]" )
        print("bart_control", request.bart_control)

    def handle_mt_bart_service_response(self, future):
        response = future.result()
        print("---------------------------" )
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt_bart_control)


    # Task Manager to Mass service call (server)
    def handle_tm_service_request(self, request, response):
        print("----------------------------------------")
        print(f"[service][receive][Mass] <- [TaskManager] : {request.robot_control_cmd}")
        print(f"[service][receive][Mass] <- [TaskManager] : {request.menu}")
        print(f"[service][receive][Mass] <- [TaskManager] : {request.menu_cnt}")
        
        response.response_tm = 10
        return response

import tkinter as tk
from tkinter import messagebox
import multiprocessing
from multiprocessing.managers import BaseManager


class gui_control(Node):
    def __init__(self):
        super().__init__('gui_control')
        self.srv_client = self.create_client(MTservice, 'MTservice')  
        self.srv_client_mt_bartcontrol = self.create_client(MTBartControl, 'MTBartControl')  

    def show_message():
        messagebox.showinfo("메시지", "버튼이 클릭되었습니다!")

    # Mass to Task Manager service call 
    def call_srv_status(self, data):
        request = MTservice.Request()
        # will be change below params using data value. 
        request.service_finish_status = 1
        request.left_dish_status = 1
        request.right_dish_status = 1

        self.srv_client.call_async(request).add_done_callback(self.handle_mt_service_response)
        print("---------------------------" )
        print("[service][request][MASS] -> [TASKmanager]" )
        print("service_finish_status", request.service_finish_status)
        print("left_dish_status", request.left_dish_status)
        print("right_dish_status", request.right_dish_status)

    def handle_mt_service_response(self, future):
        response = future.result()
        print("---------------------------" )
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt)

    # Mass to Task Manager service call 
    def call_srv_bart(self, data):
        request = MTBartControl.Request()
        # will be change below params using data value. 
        request.bart_control = 1

        self.srv_client_mt_bartcontrol.call_async(request).add_done_callback(self.handle_mt_bart_service_response)
        print("---------------------------" )
        print("[service][request][MASS] -> [TASKmanager]" )
        print("bart_control", request.bart_control)

    def handle_mt_bart_service_response(self, future):
        response = future.result()
        print("---------------------------" )
        print("[service][response][MASS]<-[TaskManager] : ", response.response_mt_bart_control)

    # 메인 윈도우 생성
    root = tk.Tk()
    root.title("MASS TEST GUI")
    root.geometry("300x200")

    # 메뉴 생성
    menubar = tk.Menu(root)
    root.config(menu=menubar)

    filemenu = tk.Menu(menubar, tearoff=0)
    menubar.add_cascade(label="파일", menu=filemenu)
    filemenu.add_command(label="종료", command=root.quit)

    # 버튼 추가
    button = tk.Button(root, text="클릭하세요", command=show_message)
    button.pack(pady=5)
    button1 = tk.Button(root, text="call_srv_status", command=call_srv_status)
    button1.pack(pady=5)
    button2 = tk.Button(root, text="call_srv_bart", command=call_srv_bart)
    button2.pack(pady=5)
    # 텍스트 레이블 추가
    label = tk.Label(root, text="MASS TEST GUI입니다.")
    label.pack(pady=10)
    # GUI 실행
    root.mainloop()

def main(args=None):

    manager = BaseManager()
    manager.start()

    rclpy.init(args=args)
    node = mass_dummy()
    gui_start = gui_control()
    gui_proc = multiprocessing.Process(target=gui_start, args=())
    gui_proc.start()
    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()
    gui_proc.kill()
    gui_proc.join()

if __name__ == '__main__':
    main()
