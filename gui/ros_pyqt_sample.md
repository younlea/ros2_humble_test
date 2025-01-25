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

출처
