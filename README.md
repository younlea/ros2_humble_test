# ros2_humble_test

https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Custom-ROS2-Interfaces.html

message를 따로 만들고 메세지를 쓸 package들을 만들어서 써야함.
한번에 넣으니까 안된다 .ㅡ.ㅡ;

1. message관련 package 만든다.
   ros2 create xxx
   mkdir msg
   touch msg/nu.msg

   CMakeList.txt 아래 내용 추가
   rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DataAB.msg"
  "msg/DataBA.msg"
  "msg/DataBC.msg"
  "msg/DataCB.msg"
  )
   package.xml 아래 내용 추가
   
  <buildtool_depend>rosidl_default_generators</buildtool_depend>
  <exec_depend>rosidl_default_runtime</exec_depend>
  <member_of_group>rosidl_interface_packages</member_of_group>

빌드 된거 확인
   ros2 interface show interface_msg/msg/DataAB 로 확인가능. 
   
2. 각 process에서 mseeage 사용
    ros2 create pkg amend_python process_xx
   
    package.xml 에 아래 내용(message관련 dependent추가)
     <depend>interface_msg</depend>

   python code에 아래 내용 추가(interface_msg/msg/DataBA 파일이 있다.)
   from interface_msg.msg import DataBA, DataBC
     
    setup.py에 아래 내용 추가.
    'console_scripts': [
         'ros2_layer_b = process_b.ros2_layer_b:main',
    ],
   
