Python 기반의 ROS 2 패키지를 `ament_python`으로 빌드할 때, `CMakeLists.txt` 파일은 일반적으로 사용되지 않습니다. 대신, Python 패키지에서는 `setup.py` 파일을 사용하여 패키지를 빌드하고 설치합니다. 하지만 메시지 파일을 포함하여 빌드할 경우, 메시지 생성과 관련된 작업이 제대로 처리되지 않는다면 이를 해결할 방법이 필요합니다.

### Python 패키지에서 메시지 빌드 문제 해결

Python 기반 ROS 2 패키지에서 메시지 파일을 빌드하려면 `CMakeLists.txt` 대신 `setup.py`와 `package.xml` 파일을 올바르게 설정해야 합니다. `ament_cmake`와 `rosidl_default_generators`를 이용한 메시지 빌드는 CMake 기반 패키지에서 사용되며, Python 패키지에서는 다른 방법으로 메시지를 빌드해야 합니다.

아래의 단계는 `ament_python`을 사용하여 Python 패키지에서 메시지를 빌드하고 사용하는 방법을 설명합니다.

### 1. 메시지 파일 구조

우선, 메시지 파일의 위치와 패키지 구조를 확인합니다. 예를 들어, `process_a` 패키지에서 `DataAB.msg` 메시지를 정의한다고 가정합니다.

```
process_a/
├── msg/
│   └── DataAB.msg
├── process_a/
│   ├── __init__.py
│   ├── algorithm_layer_a.py
│   ├── intermediate_layer_a.py
│   └── ros_layer_a.py
├── setup.py
└── package.xml
```

### 2. `setup.py` 파일 설정

`setup.py` 파일에서 `data_files` 섹션에 메시지 파일을 포함시키고, ROS 2 패키지를 인식할 수 있도록 설정합니다.

```python
from setuptools import setup
import os
from glob import glob

package_name = 'process_a'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'msg'), glob('msg/*.msg')),  # 메시지 파일 추가
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='your_email@example.com',
    description='Process A package',
    license='Apache License 2.0',
    entry_points={
        'console_scripts': [
            'ros_layer_a = process_a.ros_layer_a:main',
        ],
    },
    package_dir={'': 'src'},  # 소스 코드가 src 디렉토리에 있을 경우
)
```

### 3. `package.xml` 파일 설정

`package.xml` 파일에서 메시지 생성에 필요한 의존성을 설정합니다. 이 설정이 정확하지 않으면 메시지 파일이 제대로 빌드되지 않을 수 있습니다.

```xml
<package format="2">
  <name>process_a</name>
  <version>0.0.0</version>
  <description>Process A package</description>
  <maintainer email="your_email@example.com">Your Name</maintainer>
  <license>Apache License 2.0</license>

  <buildtool_depend>ament_python</buildtool_depend>
  <buildtool_depend>rosidl_default_generators</buildtool_depend>

  <depend>rclpy</depend>
  <exec_depend>rosidl_default_runtime</exec_depend>

  <member_of_group>rosidl_interface_packages</member_of_group>  <!-- 이 라인 추가 -->
  
  <export>
    <build_type>ament_python</build_type>
  </export>
</package>
```

### 4. 빌드 및 설치

이제 패키지를 빌드할 수 있습니다. 빌드와 설치가 성공적으로 완료되면 메시지 파일이 설치 경로에 생성됩니다.

```bash
colcon build --packages-select process_a --symlink-install
```

빌드가 완료된 후, 다음 명령어를 실행하여 환경을 설정합니다:

```bash
source install/setup.bash
```

### 5. 메시지 확인

`install` 디렉토리 아래의 `lib/python3.x/site-packages/process_a/msg/` 경로에 `DataAB.py` 파일이 생성되었는지 확인합니다.

### 6. `ros2 run` 명령어 실행

이제 `ros2 run` 명령어를 통해 노드를 실행할 수 있습니다:

```bash
ros2 run process_a ros_layer_a
```

### CMakeLists.txt 대신 setup.py 사용 이유

Python 기반 ROS 2 패키지는 기본적으로 `setup.py`를 사용하여 빌드와 설치를 관리합니다. 이 파일을 통해 Python 모듈과 ROS 2의 메시지, 서비스 등을 설정할 수 있습니다. CMake 기반 빌드 시스템인 `CMakeLists.txt`는 주로 C++ 패키지에서 사용되며, Python 패키지에서는 `ament_python`과 `setup.py`가 이를 대신합니다.

따라서 Python 패키지에서 메시지 파일이 제대로 설치되지 않으면, `setup.py`와 `package.xml` 파일 설정을 다시 한 번 점검하여 올바르게 설정되었는지 확인해야 합니다.
