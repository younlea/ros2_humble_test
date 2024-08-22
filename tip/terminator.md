# ros자동화 스크립트
```
ros2 node들을 실행시켜서 테스트를 하려고 하는데 총 7개의 node을 터미널마다 열어서 실행해야해. 실행 시키는걸 node1, node2, node3, node4, node5, node6, node7이라고하면 터미널 프로그램은 터미네이터를 써서 하고 하나의 터미네이터에 7개의 창으로 분할해서 순서대로 각 창에서 실행하는 스크립트를 만들어줘. 
node들을 실행하는건 ros2 run node1 node1 이런식으로 하면 되
```

`terminator`를 사용하여 하나의 창에서 7개의 터미널을 분할하고, 각 창에서 ROS 2 노드를 실행하는 스크립트를 작성할 수 있습니다. 아래는 그 스크립트입니다:
```
sudo apt install xdotool
```
```bash
#!/bin/bash

# Terminator를 사용하여 7개의 창으로 분할
terminator -T "ROS2 Nodes" -l base -p base -e bash -e bash -e bash -e bash -e bash -e bash -e bash &

# 잠시 대기하여 terminator 창이 뜰 때까지 기다림
sleep 2

# 창을 분할
xdotool key ctrl+shift+o  # 수평 분할
xdotool key ctrl+shift+e  # 수직 분할
xdotool key ctrl+shift+e  # 수직 분할
xdotool key ctrl+shift+e  # 수직 분할
xdotool key ctrl+shift+o  # 수평 분할
xdotool key ctrl+shift+e  # 수직 분할
xdotool key ctrl+shift+e  # 수직 분할

# 터미널 포커스를 순차적으로 이동하면서 각 노드를 실행
xdotool key alt+1
xdotool type "ros2 run node1 node1"
xdotool key Return

xdotool key alt+2
xdotool type "ros2 run node2 node2"
xdotool key Return

xdotool key alt+3
xdotool type "ros2 run node3 node3"
xdotool key Return

xdotool key alt+4
xdotool type "ros2 run node4 node4"
xdotool key Return

xdotool key alt+5
xdotool type "ros2 run node5 node5"
xdotool key Return

xdotool key alt+6
xdotool type "ros2 run node6 node6"
xdotool key Return

xdotool key alt+7
xdotool type "ros2 run node7 node7"
xdotool key Return
```

### 스크립트 설명:

1. **Terminator 실행 및 창 분할**: `terminator`를 사용해 하나의 창에서 7개의 터미널을 실행하고 분할합니다.
2. **각 노드 실행**: `xdotool`을 사용해 각 터미널에 명령을 입력하고 `Return` 키를 눌러 실행합니다.

### 사용 방법:

1. 위 스크립트를 `run_ros2_nodes.sh` 파일로 저장합니다.
2. 파일에 실행 권한을 부여합니다:
   ```bash
   chmod +x run_ros2_nodes.sh
   ```
3. 스크립트를 실행합니다:
   ```bash
   ./run_ros2_nodes.sh
   ```

이 스크립트를 실행하면 `terminator` 창이 열리고, 7개의 터미널 창으로 분할된 뒤 각 창에서 `ros2 run` 명령이 자동으로 실행됩니다.
