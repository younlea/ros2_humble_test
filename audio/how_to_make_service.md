NVIDIA Tegra에서 ROS2 기반의 `audio_playback_service`를 부팅 시 자동으로 실행되도록 설정하려면 **Systemd 서비스**를 작성해야 합니다. 아래는 Systemd 서비스를 생성하고 설정하는 방법입니다.

---

## **1. Systemd 서비스 파일 생성**

Systemd 서비스 파일은 `/etc/systemd/system/` 디렉토리에 작성됩니다. 아래는 `audio_playback_service.service` 파일의 내용입니다:

```plaintext
[Unit]
Description=Audio Playback Service
After=network.target

[Service]
ExecStart=/usr/bin/python3 /path/to/your/ros2_ws/src/audio_playback_service/audio_playback_service/audio_service.py
WorkingDirectory=/path/to/your/ros2_ws
Environment="PYTHONUNBUFFERED=1"
Restart=always
User=your_user_name

[Install]
WantedBy=multi-user.target
```

### **설명**
- **`ExecStart`**: Python 스크립트의 전체 경로를 지정합니다. ROS2 워크스페이스 내 `audio_service.py`의 경로를 정확히 입력해야 합니다.
- **`WorkingDirectory`**: ROS2 워크스페이스의 루트 디렉토리를 지정합니다.
- **`Environment="PYTHONUNBUFFERED=1"`**: Python 출력이 버퍼링되지 않도록 설정하여 로그가 즉시 기록되도록 합니다.
- **`Restart=always`**: 서비스가 종료되면 자동으로 재시작합니다.
- **`User=your_user_name`**: 서비스를 실행할 사용자 계정을 지정합니다. (예: `nvidia`)

---

## **2. 서비스 파일 저장**

1. 위 내용을 `/etc/systemd/system/audio_playback_service.service`에 저장합니다:
   ```bash
   sudo nano /etc/systemd/system/audio_playback_service.service
   ```

2. 내용을 붙여넣고 저장한 뒤 종료합니다.

---

## **3. ROS2 환경 변수 설정**

Systemd 서비스는 기본적으로 ROS2 환경 변수를 인식하지 못하므로, ROS2 워크스페이스의 환경을 로드해야 합니다.

### 방법 1: `ExecStartPre`로 ROS2 환경 로드
서비스 파일에 아래 줄을 추가하여 ROS2 환경을 로드합니다:

```plaintext
ExecStartPre=/bin/bash -c 'source /path/to/your/ros2_ws/install/setup.bash'
```

최종 서비스 파일은 다음과 같습니다:
```plaintext
[Unit]
Description=Audio Playback Service
After=network.target

[Service]
ExecStartPre=/bin/bash -c 'source /path/to/your/ros2_ws/install/setup.bash'
ExecStart=/usr/bin/python3 /path/to/your/ros2_ws/src/audio_playback_service/audio_playback_service/audio_service.py
WorkingDirectory=/path/to/your/ros2_ws
Environment="PYTHONUNBUFFERED=1"
Restart=always
User=your_user_name

[Install]
WantedBy=multi-user.target
```

---

## **4. 서비스 활성화 및 시작**

1. **Systemd 데몬 재로드**
   ```bash
   sudo systemctl daemon-reload
   ```

2. **서비스 활성화 (부팅 시 자동 실행)**
   ```bash
   sudo systemctl enable audio_playback_service.service
   ```

3. **서비스 즉시 시작**
   ```bash
   sudo systemctl start audio_playback_service.service
   ```

4. **서비스 상태 확인**
   ```bash
   sudo systemctl status audio_playback_service.service
   ```

---

## **5. 디버깅**

### 서비스가 실행되지 않을 경우:
1. **로그 확인**
   ```bash
   journalctl -u audio_playback_service.service -b
   ```
   이 명령어로 서비스 로그를 확인하여 오류 원인을 파악합니다.

2. **Python 경로 문제**
   - `ExecStart`에 지정된 Python 경로(`/usr/bin/python3`)가 올바른지 확인하세요:
     ```bash
     which python3
     ```

3. **ROS2 환경 변수 문제**
   - `/path/to/your/ros2_ws/install/setup.bash` 경로가 올바른지 확인하세요.

4. **권한 문제**
   - Python 스크립트와 관련된 모든 파일이 실행 권한을 가지고 있는지 확인하세요:
     ```bash
     chmod +x /path/to/your/ros2_ws/src/audio_playback_service/audio_playback_service/audio_service.py
     ```

---

## **6. 테스트**

1. 부팅 후 서비스를 자동으로 실행하려면 시스템을 재부팅합니다:
   ```bash
   sudo reboot now
   ```

2. 부팅 후 서비스가 실행 중인지 확인합니다:
   ```bash
   sudo systemctl status audio_playback_service.service
   ```

---

위 단계를 따르면 NVIDIA Tegra에서 `audio_playback_service`가 부팅 시 자동으로 실행되며, ROS2 기반의 음원 재생 서비스가 항상 활성 상태로 유지됩니다! 😊

출처
