아래는 ROS 2 패키지를 처음부터 생성하고, 위에서 설명한 구조로 구현하는 전체 과정을 단계별로 설명합니다.

---

## **1. ROS 2 패키지 생성**

ROS 2 패키지를 생성하려면 ROS 2 워크스페이스의 `src` 디렉토리로 이동하여 다음 명령을 실행합니다:

```bash
cd ~/ros2_ws/src
ros2 pkg create --build-type ament_python my_ros_gui_package --dependencies rclpy 
```

### **명령어 설명**:
- `--build-type ament_python`: Python 기반 ROS 2 패키지를 생성합니다.
- `--dependencies rclpy`: `rclpy`를 패키지의 의존성으로 추가합니다.
- `my_ros_gui_package`: 생성할 패키지의 이름입니다.

### **결과 디렉토리 구조**:
패키지가 생성되면 아래와 같은 기본 구조가 만들어집니다:

```
my_ros_gui_package/
├── package.xml          # ROS 2 패키지 메타데이터 파일
├── setup.py             # Python 패키지 설치 스크립트
├── setup.cfg            # 빌드 설정 파일
├── resource/
│   └── my_ros_gui_package  # 빈 리소스 파일 (필요 시 사용)
└── my_ros_gui_package/
    ├── __init__.py      # Python 패키지 초기화 파일 (빈 파일)
```

---

## **2. 코드 작성**

### **(1) PyQt5 GUI 코드**

`my_ros_gui_package/my_ros_gui_package/gui.py` 파일을 생성하고, GUI 관련 코드를 작성합니다:

```python
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget


class GuiApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 GUI")

        # UI 설정
        layout = QVBoxLayout()
        self.label = QLabel("Waiting for data...")
        layout.addWidget(self.label)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

    def update_label(self, data):
        """ROS 데이터 수신 시 UI 업데이트"""
        self.label.setText(f"Received: {data}")


def main():
    """PyQt5 GUI 실행"""
    app = QApplication(sys.argv)
    gui = GuiApp()
    gui.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
```

---

### **(2) ROS 노드 코드**

`my_ros_gui_package/my_ros_gui_package/ros_node.py` 파일을 생성하고, ROS 노드와 GUI 실행 코드를 작성합니다:

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import multiprocessing


def start_gui():
    """GUI 프로세스를 시작하는 함수"""
    from my_ros_gui_package.gui import main as gui_main
    gui_main()


class PlateTypeSubscriber(Node):
    def __init__(self):
        super().__init__('plate_type_subscriber')

        # plate_type 토픽 구독 설정
        self.subscription = self.create_subscription(
            String,
            'plate_type',
            self.listener_callback,
            10
        )
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        """토픽 데이터 수신 시 처리"""
        self.get_logger().info(f'Received: {msg.data}')
        # 여기서 IPC 또는 다른 방법으로 GUI와 데이터를 공유할 수 있음


def main(args=None):
    """ROS 노드 및 GUI 프로세스 실행"""
    rclpy.init(args=args)

    # GUI를 별도의 프로세스로 실행
    gui_process = multiprocessing.Process(target=start_gui)
    gui_process.start()

    # ROS 노드 실행
    node = PlateTypeSubscriber()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

        # GUI 프로세스 종료
        if gui_process.is_alive():
            gui_process.terminate()
            gui_process.join()


if __name__ == '__main__':
    main()
```

---

## **3. `setup.py` 수정**

`setup.py` 파일을 수정하여 ROS 노드와 GUI를 설치 가능하도록 설정합니다:

```python
from setuptools import setup

package_name = 'my_ros_gui_package'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='your_email@example.com',
    description='A ROS 2 package with integrated PyQt5 GUI',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ros_node = my_ros_gui_package.ros_node:main',
        ],
    },
)
```

---

## **4. 빌드 및 실행**

### **(1) 빌드**
ROS 2 워크스페이스로 이동한 후 패키지를 빌드합니다:

```bash
cd ~/ros2_ws
colcon build --packages-select my_ros_gui_package
```

### **(2) 실행**
빌드가 완료되면 환경을 소스하고 ROS 노드를 실행합니다:

```bash
source install/setup.bash
ros2 run my_ros_gui_package ros_node
```

---

## **5. 최종 디렉토리 구조**

최종적으로 디렉토리는 아래와 같이 구성됩니다:

```
my_ros_gui_package/
├── package.xml          # ROS 2 패키지 메타데이터 파일
├── setup.py             # Python 설치 스크립트
├── setup.cfg            # 빌드 설정 파일
├── resource/
│   └── my_ros_gui_package  # 빈 리소스 파일 (필요 시 사용)
└── my_ros_gui_package/
    ├── __init__.py      # Python 패키지 초기화 파일 (빈 파일)
    ├── ros_node.py      # ROS 노드 코드 (GUI 프로세스 실행 포함)
    └── gui.py           # PyQt5 GUI 코드 (독립적인 UI)
```

---

이제 `ros2 run my_ros_gui_package ros_node` 명령어로 ROS 노드와 PyQt5 GUI를 함께 실행할 수 있습니다. 추가적인 질문이나 요구 사항이 있다면 언제든 말씀해주세요! 😊

출처




아래는 **PyQt5 GUI를 별도의 클래스로 분리**하고, ROS 프로세스에서 GUI를 실행하도록 구현한 코드입니다. 이 방식은 ROS 2 노드가 실행될 때 GUI를 별도의 프로세스로 실행하며, 두 시스템이 독립적으로 동작하면서도 동일한 ROS 프로세스 내에서 관리됩니다.

---

## **1. 디렉토리 구조**

최종 디렉토리 구조는 다음과 같습니다:

```
my_ros_gui_package/
├── package.xml          # 패키지 메타데이터
├── setup.py             # 설치 스크립트
├── setup.cfg            # 빌드 설정 파일
├── resource/
│   └── my_ros_gui_package  # 빈 리소스 파일 (필요 시 사용)
└── my_ros_gui_package/
    ├── __init__.py       # 패키지 초기화 파일 (빈 파일)
    ├── ros_node.py       # ROS 노드 코드 (GUI 프로세스 실행 포함)
    └── gui.py            # PyQt5 GUI 코드
```

---

## **2. PyQt5 GUI 코드**

`my_ros_gui_package/my_ros_gui_package/gui.py`:

```python
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget


class GuiApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 GUI")

        # UI 설정
        layout = QVBoxLayout()
        self.label = QLabel("Waiting for data...")
        layout.addWidget(self.label)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

    def update_label(self, data):
        """ROS 데이터 수신 시 UI 업데이트"""
        self.label.setText(f"Received: {data}")


def main():
    """PyQt5 GUI 실행"""
    app = QApplication(sys.argv)
    gui = GuiApp()
    gui.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
```

---

## **3. ROS 노드 코드**

`my_ros_gui_package/my_ros_gui_package/ros_node.py`:

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import multiprocessing
import time


def start_gui():
    """GUI 프로세스를 시작하는 함수"""
    from my_ros_gui_package.gui import main as gui_main
    gui_main()


class PlateTypeSubscriber(Node):
    def __init__(self):
        super().__init__('plate_type_subscriber')

        # plate_type 토픽 구독 설정
        self.subscription = self.create_subscription(
            String,
            'plate_type',
            self.listener_callback,
            10
        )
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        """토픽 데이터 수신 시 처리"""
        self.get_logger().info(f'Received: {msg.data}')
        # 여기서 IPC 또는 다른 방법으로 GUI와 데이터를 공유할 수 있음


def main(args=None):
    """ROS 노드 및 GUI 프로세스 실행"""
    rclpy.init(args=args)

    # GUI를 별도의 프로세스로 실행
    gui_process = multiprocessing.Process(target=start_gui)
    gui_process.start()

    # ROS 노드 실행
    node = PlateTypeSubscriber()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

        # GUI 프로세스 종료
        if gui_process.is_alive():
            gui_process.terminate()
            gui_process.join()


if __name__ == '__main__':
    main()
```

---

## **4. 주요 구현 설명**

1. **GUI 분리**:
   - `GuiApp` 클래스는 별도의 `gui.py` 파일에 정의되어 있습니다.
   - `gui.py`의 `main()` 함수는 PyQt5 애플리케이션을 실행합니다.

2. **멀티프로세싱**:
   - Python의 `multiprocessing.Process`를 사용하여 `start_gui()` 함수를 별도의 프로세스로 실행합니다.
   - 이 방식은 ROS와 GUI가 독립적으로 동작하며, 서로 영향을 주지 않습니다.

3. **ROS 노드**:
   - `PlateTypeSubscriber` 클래스는 `plate_type` 토픽을 구독하며 데이터를 처리합니다.
   - 필요 시 IPC(예: 소켓, 공유 메모리 등)를 통해 ROS와 GUI 간 데이터를 교환할 수 있습니다.

4. **프로세스 종료**:
   - ROS 노드가 종료될 때(`KeyboardInterrupt`) GUI 프로세스도 안전하게 종료됩니다.

---

## **5. `setup.py` 수정**

`setup.py` 파일을 수정하여 패키지를 설치 가능하도록 설정합니다:

```python
from setuptools import setup

package_name = 'my_ros_gui_package'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='your_email@example.com',
    description='A ROS 2 package with integrated PyQt5 GUI',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ros_node = my_ros_gui_package.ros_node:main',
        ],
    },
)
```

---

## **6. 빌드 및 실행**

### **(1) 빌드**
ROS 2 워크스페이스로 이동한 후 패키지를 빌드합니다:

```bash
cd ~/ros2_ws
colcon build --packages-select my_ros_gui_package
```

### **(2) 실행**
빌드가 완료되면 환경을 소스하고 ROS 노드를 실행합니다:

```bash
source install/setup.bash
ros2 run my_ros_gui_package ros_node
```

---

## **7. 결과**

- 터미널에서 `ros2 run my_ros_gui_package ros_node`를 실행하면:
  - ROS 2 노드는 `plate_type` 토픽을 구독합니다.
  - PyQt5 GUI가 별도의 창으로 열립니다.
  - 두 시스템은 독립적인 프로세스로 동작하며, 필요 시 데이터를 교환할 수 있습니다.

이 방식은 ROS와 PyQt5를 명확히 분리하고 멀티프로세싱을 활용하여 안정성을 높입니다. 추가적인 요구 사항이 있다면 말씀해주세요! 😊

아래는 ROS 2 노드에서 `plate_type`, `plate_set`, `vat_tilt`, `vat_status` 토픽을 구독하고, 해당 데이터를 **PyQt5 GUI 프로세스**로 전달하는 코드를 작성한 예제입니다. ROS 노드와 GUI 간의 데이터 전달은 **멀티프로세싱의 Queue**를 사용하여 구현합니다.

---

## **1. 코드 구조**

### **디렉토리 구조**
```
my_ros_gui_package/
├── package.xml          # ROS 2 패키지 메타데이터
├── setup.py             # Python 설치 스크립트
├── setup.cfg            # 빌드 설정 파일
├── resource/
│   └── my_ros_gui_package  # 빈 리소스 파일 (필요 시 사용)
└── my_ros_gui_package/
    ├── __init__.py      # Python 패키지 초기화 파일 (빈 파일)
    ├── ros_node.py      # ROS 노드 코드 (GUI 프로세스 실행 포함)
    └── gui.py           # PyQt5 GUI 코드
```

---

## **2. PyQt5 GUI 코드**

`my_ros_gui_package/my_ros_gui_package/gui.py`:

```python
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget
from PyQt5.QtCore import QTimer


class GuiApp(QMainWindow):
    def __init__(self, data_queue):
        super().__init__()
        self.setWindowTitle("PyQt5 GUI")
        self.data_queue = data_queue

        # UI 설정
        layout = QVBoxLayout()
        self.label_plate_type = QLabel("plate_type: Waiting...")
        self.label_plate_set = QLabel("plate_set: Waiting...")
        self.label_vat_tilt = QLabel("vat_tilt: Waiting...")
        self.label_vat_status = QLabel("vat_status: Waiting...")

        layout.addWidget(self.label_plate_type)
        layout.addWidget(self.label_plate_set)
        layout.addWidget(self.label_vat_tilt)
        layout.addWidget(self.label_vat_status)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        # Timer to check for new data from the queue
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_labels)
        self.timer.start(100)  # Check every 100 ms

    def update_labels(self):
        """Check the queue for new data and update the labels."""
        while not self.data_queue.empty():
            topic, value = self.data_queue.get()
            if topic == "plate_type":
                self.label_plate_type.setText(f"plate_type: {value}")
            elif topic == "plate_set":
                self.label_plate_set.setText(f"plate_set: {value}")
            elif topic == "vat_tilt":
                self.label_vat_tilt.setText(f"vat_tilt: {value}")
            elif topic == "vat_status":
                self.label_vat_status.setText(f"vat_status: {value}")


def main(data_queue):
    """PyQt5 GUI 실행"""
    app = QApplication(sys.argv)
    gui = GuiApp(data_queue)
    gui.show()
    sys.exit(app.exec_())
```

---

## **3. ROS 노드 코드**

`my_ros_gui_package/my_ros_gui_package/ros_node.py`:

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import multiprocessing


def start_gui(data_queue):
    """GUI 프로세스를 시작하는 함수"""
    from my_ros_gui_package.gui import main as gui_main
    gui_main(data_queue)


class MultiTopicSubscriber(Node):
    def __init__(self, data_queue):
        super().__init__('multi_topic_subscriber')
        self.data_queue = data_queue

        # 여러 토픽 구독 설정
        self.create_subscription(String, 'plate_type', self.listener_callback_plate_type, 10)
        self.create_subscription(String, 'plate_set', self.listener_callback_plate_set, 10)
        self.create_subscription(String, 'vat_tilt', self.listener_callback_vat_tilt, 10)
        self.create_subscription(String, 'vat_status', self.listener_callback_vat_status, 10)

    def listener_callback_plate_type(self, msg):
        """plate_type 토픽 데이터 처리"""
        self.data_queue.put(("plate_type", msg.data))
        self.get_logger().info(f"Received plate_type: {msg.data}")

    def listener_callback_plate_set(self, msg):
        """plate_set 토픽 데이터 처리"""
        self.data_queue.put(("plate_set", msg.data))
        self.get_logger().info(f"Received plate_set: {msg.data}")

    def listener_callback_vat_tilt(self, msg):
        """vat_tilt 토픽 데이터 처리"""
        self.data_queue.put(("vat_tilt", msg.data))
        self.get_logger().info(f"Received vat_tilt: {msg.data}")

    def listener_callback_vat_status(self, msg):
        """vat_status 토픽 데이터 처리"""
        self.data_queue.put(("vat_status", msg.data))
        self.get_logger().info(f"Received vat_status: {msg.data}")


def main(args=None):
    """ROS 노드 및 GUI 프로세스 실행"""
    rclpy.init(args=args)

    # 멀티프로세싱 큐 생성 (ROS와 GUI 간 데이터 공유)
    data_queue = multiprocessing.Queue()

    # GUI를 별도의 프로세스로 실행
    gui_process = multiprocessing.Process(target=start_gui, args=(data_queue,))
    gui_process.start()

    # ROS 노드 실행
    node = MultiTopicSubscriber(data_queue)
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

        # GUI 프로세스 종료
        if gui_process.is_alive():
            gui_process.terminate()
            gui_process.join()


if __name__ == '__main__':
    main()
```

---

## **4. 주요 구현 설명**

1. **멀티프로세싱 큐**:
   - `multiprocessing.Queue`를 사용하여 ROS 노드에서 수신한 데이터를 PyQt5 GUI로 전달합니다.
   - ROS 노드는 각 토픽의 데이터를 큐에 추가하며, GUI는 주기적으로 큐를 확인하여 데이터를 가져옵니다.

2. **ROS 및 PyQt5 통합**:
   - `MultiTopicSubscriber` 클래스는 네 개의 토픽(`plate_type`, `plate_set`, `vat_tilt`, `vat_status`)을 구독합니다.
   - 각 콜백 함수는 수신된 메시지를 큐에 추가합니다.

3. **GUI 업데이트**:
   - PyQt5의 `QTimer`를 사용하여 주기적으로 큐를 확인하고 데이터를 UI에 반영합니다.

4. **프로세스 관리**:
   - ROS 노드와 PyQt5 GUI는 별도의 프로세스로 실행되며 독립적으로 동작합니다.
   - ROS 노드 종료 시 GUI 프로세스도 안전하게 종료됩니다.

---

## **5. 빌드 및 실행**

### **(1) 빌드**
ROS 2 워크스페이스로 이동한 후 패키지를 빌드합니다:

```bash
cd ~/ros2_ws
colcon build --packages-select my_ros_gui_package
```

### **(2) 실행**
빌드가 완료되면 환경을 소스하고 ROS 노드를 실행합니다:

```bash
source install/setup.bash
ros2 run my_ros_gui_package ros_node
```

---

## **6. 결과**

- 터미널에서 `ros2 run my_ros_gui_package ros_node`를 실행하면:
  - ROS 2 노드는 네 개의 토픽(`plate_type`, `plate_set`, `vat_tilt`, `vat_status`)을 구독합니다.
  - PyQt5 GUI가 별도의 창으로 열립니다.
  - 각 토픽에서 수신된 메시지가 GUI에 실시간으로 표시됩니다.

추가적인 요구 사항이 있다면 언제든 말씀해주세요! 😊

출처
[1] ros-tooling/topic_tools - GitHub https://github.com/ros-tooling/topic_tools
[2] Setting up efficient intra-process communication - ROS Documentation https://docs.ros.org/en/foxy/Tutorials/Demos/Intra-Process-Communication.html
[3] Using ROS message and python to update text input field in kivy GUI https://answers.ros.org/question/174905/using-ros-message-and-python-to-update-text-input-field-in-kivy-gui/
[4] How to set/get parameters from another node | ROS 2 Humble ... https://www.theconstruct.ai/how-to-set-get-parameters-from-another-node-ros2-humble-python-tutorial/
[5] Communication between nodes ROS (Python) - Stack Overflow https://stackoverflow.com/questions/37371898/communication-between-nodes-ros-python
[6] Use data from multiple topics in ROS - Python - Stack Overflow https://stackoverflow.com/questions/48830056/use-data-from-multiple-topics-in-ros-python
[7] How to share data between two different processes in Linux? https://robotics.stackexchange.com/questions/111876/how-to-share-data-between-two-different-processes-in-linux
[8] Create a ROS2 Service: Server and Client Nodes in Python and C++ https://www.roboticsunveiled.com/ros2-service-server-and-client-python-and-cpp/
[9] Writing a simple service and client (Python) — ROS 2 Documentation https://docs.ros.org/en/foxy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Py-Service-And-Client.html
[10] ROS 2 Python Custom Messages — Omniverse IsaacSim - NVIDIA https://docs.omniverse.nvidia.com/isaacsim/latest/ros2_tutorials/tutorial_ros2_custom_message_python.html
[11] Publishers and Subscribers: using messages - ROS2 Tutorial https://ros2-tutorial.readthedocs.io/en/latest/publishers_and_subscribers.html
[12] How Can I Display a Message From a ROS2 Publisher In a KivyMD ... https://stackoverflow.com/questions/72974005/how-can-i-display-a-message-from-a-ros2-publisher-in-a-kivymd-window
[13] Intra-process Communications in ROS 2 https://design.ros2.org/articles/intraprocess_communications.html
[14] ROS2 - (5) Python을 이용한 Topic, Service 구현 - 잇!(IT) 블로그 https://insoobaik.tistory.com/709
[15] Efficient intra-process communication — ROS 2 Documentation https://docs.ros.org/en/eloquent/Tutorials/Intra-Process-Communication.html
[16] Accessing topics from multiple ros2 workspaces, sourced separately https://answers.ros.org/question/405045/
[17] rostopic - Python Multiprocessing to publish to multiple topics https://robotics.stackexchange.com/questions/56644/python-multiprocessing-to-publish-to-multiple-topics
[18] ROS and ROS 2 — Omniverse IsaacSim - NVIDIA https://docs.omniverse.nvidia.com/isaacsim/latest/ros_ros2_tutorials.html
[19] Tutorial 1: ROS basic concepts and development tools https://sir.upc.edu/projects/ros2tutorials/1-ROS_basic_concepts_and_development_tools/index.html
[20] Efficient intra-process communication — ROS 2 Documentation https://daobook.github.io/ros2-docs/xin/Tutorials/Intra-Process-Communication.html
[21] Evaluating and deploying ROS 2 nodes for real-time AI computer ... https://antmicro.com/blog/2024/05/deploying-ros-2-nodes-for-ai-cv-with-kenning/
[22] How to Create a ROS 2 Python Publisher – Jazzy - Automatic Addison https://automaticaddison.com/how-to-create-a-ros-2-python-publisher-jazzy/


Python의 `multiprocessing.shared_memory` 모듈을 사용하여 **공유 메모리**를 생성하고, ROS 노드에서 데이터를 업데이트하며 GUI에서 읽기만 하는 구조를 구현할 수 있습니다. 아래는 이를 구현하는 방법입니다.

---

## **1. 공유 메모리 기반 설계**

### **구조**
1. **ROS 노드**:
   - `SharedMemory`를 사용하여 공유 메모리를 생성합니다.
   - ROS 토픽 데이터를 수신하면 공유 메모리를 업데이트합니다.

2. **GUI 프로세스**:
   - 공유 메모리에 연결하여 데이터를 읽습니다.
   - 주기적으로 데이터를 확인하여 GUI를 업데이트합니다.

---

## **2. 코드 구현**

### **(1) 공유 메모리 초기화**
공유 메모리는 `multiprocessing.shared_memory.SharedMemory` 클래스를 사용하여 생성합니다. 각 데이터는 고유한 이름을 가진 공유 메모리 블록에 저장됩니다.

### **(2) ROS 노드 코드**

`ros_node.py`:

```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from multiprocessing import shared_memory
import numpy as np


class SharedMemoryManager:
    """공유 메모리 관리 클래스"""
    def __init__(self):
        # 공유 메모리 생성 (4개의 float 값을 저장)
        self.shm_plate_type = shared_memory.SharedMemory(create=True, size=8, name="plate_type")
        self.shm_plate_set = shared_memory.SharedMemory(create=True, size=8, name="plate_set")
        self.shm_vat_tilt = shared_memory.SharedMemory(create=True, size=8, name="vat_tilt")
        self.shm_vat_status = shared_memory.SharedMemory(create=True, size=8, name="vat_status")

    def update_shared_memory(self, shm_name: str, value: float):
        """특정 공유 메모리를 업데이트"""
        shm = shared_memory.SharedMemory(name=shm_name)
        np_array = np.ndarray((1,), dtype=np.float64, buffer=shm.buf)
        np_array[0] = value

    def cleanup(self):
        """공유 메모리 해제"""
        self.shm_plate_type.close()
        self.shm_plate_set.close()
        self.shm_vat_tilt.close()
        self.shm_vat_status.close()
        self.shm_plate_type.unlink()
        self.shm_plate_set.unlink()
        self.shm_vat_tilt.unlink()
        self.shm_vat_status.unlink()


class FacilityStatusSubscriber(Node):
    def __init__(self, shm_manager):
        super().__init__('facility_status_subscriber')
        self.shm_manager = shm_manager

        # ROS 토픽 구독 설정
        self.create_subscription(String, 'plate_type', self.plate_type_callback, 10)
        self.create_subscription(String, 'plate_set', self.plate_set_callback, 10)
        self.create_subscription(String, 'vat_tilt', self.vat_tilt_callback, 10)
        self.create_subscription(String, 'vat_status', self.vat_status_callback, 10)

    def plate_type_callback(self, msg):
        value = float(msg.data)
        self.get_logger().info(f"Received plate_type: {value}")
        self.shm_manager.update_shared_memory("plate_type", value)

    def plate_set_callback(self, msg):
        value = float(msg.data)
        self.get_logger().info(f"Received plate_set: {value}")
        self.shm_manager.update_shared_memory("plate_set", value)

    def vat_tilt_callback(self, msg):
        value = float(msg.data)
        self.get_logger().info(f"Received vat_tilt: {value}")
        self.shm_manager.update_shared_memory("vat_tilt", value)

    def vat_status_callback(self, msg):
        value = float(msg.data)
        self.get_logger().info(f"Received vat_status: {value}")
        self.shm_manager.update_shared_memory("vat_status", value)


def main(args=None):
    rclpy.init(args=args)
    shm_manager = SharedMemoryManager()

    node = FacilityStatusSubscriber(shm_manager)
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        shm_manager.cleanup()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
```

---

### **(3) GUI 코드**

`gui.py`:

```python
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QWidget
from PyQt5.QtCore import QTimer
from multiprocessing import shared_memory
import numpy as np


class GuiApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt5 GUI with Shared Memory")

        # UI 설정
        layout = QVBoxLayout()
        self.label_plate_type = QLabel("plate_type: Waiting...")
        self.label_plate_set = QLabel("plate_set: Waiting...")
        self.label_vat_tilt = QLabel("vat_tilt: Waiting...")
        self.label_vat_status = QLabel("vat_status: Waiting...")

        layout.addWidget(self.label_plate_type)
        layout.addWidget(self.label_plate_set)
        layout.addWidget(self.label_vat_tilt)
        layout.addWidget(self.label_vat_status)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        # 공유 메모리 연결
        self.shm_plate_type = shared_memory.SharedMemory(name="plate_type")
        self.shm_plate_set = shared_memory.SharedMemory(name="plate_set")
        self.shm_vat_tilt = shared_memory.SharedMemory(name="vat_tilt")
        self.shm_vat_status = shared_memory.SharedMemory(name="vat_status")

        # Timer 설정 (100ms마다 데이터 읽기)
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_labels)
        self.timer.start(100)

    def update_labels(self):
        """공유 메모리에서 데이터를 읽어와 UI를 업데이트"""
        
       # 각각의 값 읽기 
       for (name,label) in zip(["shm_plate_type","shm_plate...

출처
[1] multiprocessing.shared_memory — Shared memory for direct ... https://docs.python.org/3/library/multiprocessing.shared_memory.html
[2] Shared Memory in python - Omid Sadeghnezhad https://sadeghnezhad.me/blog/2024/shared-memory-python/
[3] Inter-Process Communication (IPC) in Python [with Examples] | Apriorit https://www.apriorit.com/dev-blog/web-python-ipc-methods
[4] multiprocessing.shared_memory — 프로세스 간 직접 액세스를 위한 ... https://docs.python.org/ko/3.9/library/multiprocessing.shared_memory.html
[5] [Python] shared memory 활용하여 프로세스 간 이미지 공유하기 https://whiteknight3672.tistory.com/338
[6] Python Shared Memory in Multiprocessing - Mingze Gao https://mingze-gao.com/posts/python-shared-memory-in-multiprocessing/
[7] Python Process - velog https://velog.io/@tritny6516/Python-Process
[8] Shared memory in multiprocessing - python - Stack Overflow https://stackoverflow.com/questions/14124588/shared-memory-in-multiprocessing
[9] High-Performance Inter-Process Communication Between C and ... https://rafalkwasny.com/message-queue-c-python-lmax-disruptor
[10] [Python] Process간 Numpy Array 공유하기 -2편 - DevOcean - SK https://devocean.sk.com/blog/techBoardDetail.do?ID=163675
[11] Integrated C++ and Python High-Performance Computing System https://www.linkedin.com/pulse/integrated-c-python-high-performance-computing-system-matthew-denman-uic3c
[12] multiprocessing.shared_memory — 프로세스 간 직접 액세스를 위한 ... https://docs.python.org/ko/3.9/library/multiprocessing.shared_memory.html
[13] Using Shared Memory in CUDA C/C++ | NVIDIA Technical Blog https://developer.nvidia.com/blog/using-shared-memory-cuda-cc/
[14] [multiprocessing] shared memory - velog https://velog.io/@jk01019/multiprocessing-shared-memory







