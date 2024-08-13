메시지 선언은 ROS 2 패키지의 `msg` 디렉토리에서 `.msg` 파일로 정의됩니다. 각 프로세스(`process_a`, `process_b`, `process_c`)에서 사용하는 메시지를 각각의 `msg` 디렉토리에 선언합니다. 

### 1. 메시지 파일 정의

각 프로세스의 디렉토리 구조에 `msg` 디렉토리를 추가하여 메시지 타입을 정의합니다. 예를 들어, `process_a`에서는 `DataAB` 메시지를 정의할 수 있습니다.

#### `process_a` 메시지 정의

디렉토리 구조:
```
process_a/
├── msg/
│   └── DataAB.msg
├── process_a/
│   ├── algorithm_layer_a.py
│   ├── intermediate_layer_a.py
│   └── ros_layer_a.py
├── setup.py
└── package.xml
```

**DataAB.msg**:
```plaintext
float32 value1
int32 value2
```

이 메시지는 `float32`와 `int32` 타입의 필드를 갖는 단순한 메시지입니다.

### 2. `package.xml` 및 `setup.py` 설정

메시지 파일을 정의한 후, 이를 빌드할 수 있도록 `package.xml`과 `setup.py` 파일을 업데이트해야 합니다.

#### `package.xml` 업데이트

`package.xml`에 `rosidl_default_generators`와 `rosidl_default_runtime`을 추가합니다:

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

  <export>
    <build_type>ament_python</build_type>
  </export>
</package>
```

#### `setup.py` 업데이트

`setup.py`에 메시지 파일을 빌드할 수 있도록 `data_files`를 설정합니다:

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
        (os.path.join('share', package_name, 'msg'), glob('msg/*.msg')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Your Name',
    maintainer_email='your_email@example.com',
    description='Process A package',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ros_layer_a = process_a.ros_layer_a:main',
        ],
    },
)
```

### 3. 빌드 설정
`CMakeLists.txt`가 아닌 Python 기반의 ROS 2 패키지에서 메시지를 정의하려면, `rosidl_default_generators`와 같은 도구가 사용됩니다. Python 패키지에서는 `CMakeLists.txt` 대신 `setup.py`와 `package.xml`이 이 역할을 수행합니다.

메시지를 컴파일하고 이를 패키지의 일부로 포함시키기 위해서는 위와 같이 `setup.py`에 메시지 파일을 포함시키고, `package.xml`에 필요한 의존성을 명시해야 합니다.

### 4. 빌드 및 실행

이제 메시지를 포함하여 패키지를 빌드하고 실행할 수 있습니다.

1. 빌드:
   ```bash
   colcon build --packages-select process_a
   ```

2. 환경 설정 후 실행:
   ```bash
   source install/setup.bash
   ros2 run process_a ros_layer_a
   ```

### 5. 다른 프로세스(B, C)에서 메시지 정의
각 프로세스(`process_b`, `process_c`)에서도 동일한 방법으로 `msg` 디렉토리를 생성하고 필요한 메시지 파일을 정의합니다. 메시지 파일들은 `setup.py`와 `package.xml`에 위와 같이 추가되어야 합니다.

이 구조로 메시지를 선언하면 각 프로세스 간의 메시지 통신을 설정할 수 있으며, 각 레이어를 별도의 파일로 나누어 관리할 수 있습니다.
