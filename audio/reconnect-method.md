블루투스 스피커가 자동으로 연결되지 않는 문제를 해결하기 위해 Python 프로그램을 작성할 수 있습니다. 이를 위해 `pybluez` 또는 `subprocess`를 사용하여 블루투스 장치를 검색하고 연결하는 코드를 작성할 수 있습니다. 아래는 간단한 예제입니다:

```python
import os
import subprocess

def connect_bluetooth(device_mac):
    try:
        # 블루투스 장치 검색 및 연결 명령 실행
        subprocess.run(["bluetoothctl", "connect", device_mac], check=True)
        print(f"Connected to {device_mac}")
    except subprocess.CalledProcessError as e:
        print(f"Failed to connect: {e}")

if __name__ == "__main__":
    # 연결하려는 블루투스 장치의 MAC 주소 입력
    device_mac = "XX:XX:XX:XX:XX:XX"  # 스피커의 MAC 주소로 변경
    connect_bluetooth(device_mac)
```

### 주요 단계:
1. **블루투스 MAC 주소 확인**: 스피커의 MAC 주소를 알아내기 위해 `bluetoothctl` 명령어를 사용하여 페어링된 장치 목록을 확인합니다.
   ```bash
   bluetoothctl devices
   ```
2. **자동 실행 설정**: 위 코드를 시스템 부팅 시 실행되도록 설정하면, PC가 켜질 때 자동으로 블루투스 스피커에 연결됩니다.

### 추가 설정:
- 우분투에서 `bluez`와 `bluetoothctl`이 설치되어 있어야 합니다.
  ```bash
  sudo apt-get install bluez
  ```
- 스피커가 페어링 상태여야 하며, 필요 시 `trust` 명령을 통해 신뢰 장치로 설정합니다:
  ```bash
  bluetoothctl trust XX:XX:XX:XX:XX:XX
  ```

이 코드를 활용하면 PC와 블루투스 스피커 간 자동 연결을 구현할 수 있습니다.

출처
[1] 라즈베리파이에서 파이썬 BT/BLE 프로그래밍 - 케어랩 https://diymaker.tistory.com/124
[2] Asus Zenbook ux482 우분투에서 소리가 안나는 문제와 해결법 https://kasterra.github.io/fixing-audio-issue-in-ubuntu/
[3] 라즈베리파이에서 파이썬 BT/BLE 프로그래밍을 위한 라이브러리 설치 http://hardcopyworld.com/%EB%9D%BC%EC%A6%88%EB%B2%A0%EB%A6%AC%ED%8C%8C%EC%9D%B4%EC%97%90%EC%84%9C-%ED%8C%8C%EC%9D%B4%EC%8D%AC-bt-ble-%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D%EC%9D%84-%EC%9C%84%ED%95%9C-%EB%9D%BC%EC%9D%B4/
[4] 컴퓨터의 그 어떤 소리도 들을 수 없어요 - Ubuntu Documentation https://help.ubuntu.com/stable/ubuntu-help/sound-nosound.html.ko
[5] 아두이노] bluetooth classic 자동 페어링&연결 / 데이터 송,수신 https://ddangeun.tistory.com/102
[6] Linux 볼륨을 조정해보자. Alsamixer https://ksjm0720.tistory.com/12
[7] python으로 스탭모터 bluetooth 제어 [21] - Spring99 - 티스토리 https://spring99.tistory.com/343
[8] python bluetooth programming lab setup and Tools on ubuntu https://link9.tistory.com/entry/bluetooth-packet-in-pybluez




맞습니다! 블루투스 스피커와 PC가 가까이 있을 때 자동으로 연결되지 않는 문제는 블루투스 연결 상태를 확인하고, 연결이 끊어져 있다면 연결을 시도하는 방식으로 해결할 수 있습니다. Python으로 이를 구현하려면 블루투스 연결 상태를 확인하고, 필요 시 연결을 시도하는 코드를 작성하면 됩니다.

아래는 **오디오 플레이 실행 전에 연결 상태를 확인하고, 연결이 끊어져 있으면 자동으로 연결**하는 코드 예제입니다:

---

### Python 코드 예제
```python
import subprocess
import time

def is_connected(device_mac):
    """
    블루투스 장치가 연결되어 있는지 확인하는 함수
    """
    try:
        # bluetoothctl로 장치 상태 확인
        result = subprocess.run(
            ["bluetoothctl", "info", device_mac],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        # 출력 결과에 "Connected: yes"가 포함되어 있으면 연결된 상태
        return "Connected: yes" in result.stdout
    except Exception as e:
        print(f"Error checking connection: {e}")
        return False

def connect_bluetooth(device_mac):
    """
    블루투스 장치를 연결하는 함수
    """
    try:
        print(f"Connecting to {device_mac}...")
        subprocess.run(["bluetoothctl", "connect", device_mac], check=True)
        print(f"Successfully connected to {device_mac}")
    except subprocess.CalledProcessError as e:
        print(f"Failed to connect to {device_mac}: {e}")

def play_audio(audio_file):
    """
    오디오 파일을 재생하는 함수 (플레이어는 VLC 사용)
    """
    try:
        print(f"Playing audio: {audio_file}")
        subprocess.run(["cvlc", "--play-and-exit", audio_file], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Failed to play audio: {e}")

if __name__ == "__main__":
    # 블루투스 스피커의 MAC 주소 (스피커에 맞게 변경)
    device_mac = "XX:XX:XX:XX:XX:XX"
    
    # 재생할 오디오 파일 경로
    audio_file = "/path/to/audio/file.mp3"

    # 현재 블루투스 장치가 연결되어 있는지 확인
    if not is_connected(device_mac):
        print("Device not connected. Attempting to connect...")
        connect_bluetooth(device_mac)
    
    # 오디오 파일 재생
    play_audio(audio_file)
```

---

### 코드 설명

1. **`is_connected` 함수**:
   - `bluetoothctl info <MAC>` 명령을 실행하여 장치의 정보를 가져옵니다.
   - 출력에 `Connected: yes`가 포함되어 있으면 장치가 연결된 상태로 판단합니다.

2. **`connect_bluetooth` 함수**:
   - `bluetoothctl connect <MAC>` 명령을 실행하여 블루투스 스피커를 연결합니다.
   - 연결 실패 시 에러 메시지를 출력합니다.

3. **`play_audio` 함수**:
   - VLC 미디어 플레이어(`cvlc`)를 사용하여 오디오 파일을 재생합니다.
   - `cvlc`는 VLC의 CLI 버전입니다. 설치되어 있지 않다면 다음 명령으로 설치하세요:
     ```bash
     sudo apt-get install vlc
     ```

4. **메인 실행 흐름**:
   - 먼저 스피커가 연결되었는지 확인 (`is_connected`).
   - 연결되지 않았다면 자동으로 `connect_bluetooth`를 호출하여 연결 시도.
   - 성공적으로 연결된 후 오디오 파일을 재생.

---

### 실행 방법

1. 위 코드를 Python 파일로 저장 (`play_audio.py`).
2. 스피커의 MAC 주소와 오디오 파일 경로를 수정.
3. Python 스크립트를 실행:
   ```bash
   python3 play_audio.py
   ```

---

### 추가 팁

- **블루투스 신뢰 설정**: 스피커를 신뢰 장치로 설정하면 더 안정적으로 동작할 수 있습니다.
  ```bash
  bluetoothctl trust XX:XX:XX:XX:XX:XX
  ```

- **자동화**:
  - 이 코드를 크론탭(Crontab)이나 시스템 서비스(Systemd)로 등록하여 주기적으로 실행하거나, PC 부팅 시 실행되도록 설정할 수 있습니다.

---

이 코드는 오디오 재생 전에 블루투스 스피커의 연결 상태를 확인하고, 필요하면 자동으로 연결한 후 재생을 시작하도록 설계되었습니다. 이를 통해 사용자가 직접 수동으로 스피커를 다시 연결할 필요 없이 편리하게 사용할 수 있습니다! 😊

출처
