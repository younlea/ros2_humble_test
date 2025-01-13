아래는 Modbus 데이터를 주기적으로 읽어와 **MQTT**와 **REST API**를 통해 전송하는 Python 코드입니다. 두 방식 모두 주기적으로 데이터를 읽고 전송하도록 구성되어 있습니다.

---

## **1. Modbus 데이터를 MQTT로 전송하는 코드**

이 코드는 `pymodbus` 라이브러리로 Modbus 데이터를 읽고, `paho-mqtt` 라이브러리를 사용하여 MQTT 브로커로 데이터를 전송합니다.

```python
import time
import paho.mqtt.client as mqtt
from pymodbus.client import ModbusTcpClient

# MODBUS 설정
CCTV_IP = '192.168.0.100'
MODBUS_PORT = 502
START_ADDRESS = 0x0000
REGISTER_COUNT = 10
UNIT_ID = 1

# MQTT 설정
MQTT_BROKER = 'mqtt.example.com'  # MQTT 브로커 주소
MQTT_PORT = 1883                  # MQTT 포트 (기본: 1883)
MQTT_TOPIC = 'modbus/data'        # MQTT 토픽 이름

# MODBUS 클라이언트 생성
modbus_client = ModbusTcpClient(CCTV_IP, port=MODBUS_PORT)

# MQTT 클라이언트 생성 및 연결
mqtt_client = mqtt.Client()
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

# 주기적으로 데이터 읽고 MQTT로 전송
def read_and_publish():
    if modbus_client.connect():
        print("Connected to MODBUS server")
        while True:
            response = modbus_client.read_holding_registers(START_ADDRESS, REGISTER_COUNT, unit=UNIT_ID)
            if response.isError():
                print("Error reading data from MODBUS")
            else:
                data = response.registers
                print("Data read from MODBUS:", data)
                mqtt_client.publish(MQTT_TOPIC, str(data))  # 데이터를 문자열로 변환하여 전송
            time.sleep(5)  # 5초 간격으로 데이터 전송
    else:
        print("Failed to connect to MODBUS server")

# 실행
try:
    read_and_publish()
except KeyboardInterrupt:
    print("Stopped by user")
finally:
    modbus_client.close()
    mqtt_client.disconnect()
```

### **필요한 라이브러리 설치**
```bash
pip install pymodbus paho-mqtt
```

---

## **2. Modbus 데이터를 REST API로 전송하는 코드**

이 코드는 `pymodbus` 라이브러리로 데이터를 읽고, `requests` 라이브러리를 사용하여 REST API 서버로 데이터를 POST 요청으로 전송합니다.

```python
import time
import requests
from pymodbus.client import ModbusTcpClient

# MODBUS 설정
CCTV_IP = '192.168.0.100'
MODBUS_PORT = 502
START_ADDRESS = 0x0000
REGISTER_COUNT = 10
UNIT_ID = 1

# REST API 설정
API_URL = 'http://api.example.com/modbus/data'  # REST API 엔드포인트 URL
HEADERS = {'Content-Type': 'application/json'}  # JSON 형식 헤더

# MODBUS 클라이언트 생성
modbus_client = ModbusTcpClient(CCTV_IP, port=MODBUS_PORT)

# 주기적으로 데이터 읽고 REST API로 전송
def read_and_send():
    if modbus_client.connect():
        print("Connected to MODBUS server")
        while True:
            response = modbus_client.read_holding_registers(START_ADDRESS, REGISTER_COUNT, unit=UNIT_ID)
            if response.isError():
                print("Error reading data from MODBUS")
            else:
                data = response.registers
                print("Data read from MODBUS:", data)
                payload = {"data": data}  # JSON 형식으로 데이터 구성
                try:
                    response = requests.post(API_URL, json=payload, headers=HEADERS)
                    if response.status_code == 200:
                        print("Data sent successfully to REST API")
                    else:
                        print(f"Failed to send data: {response.status_code}, {response.text}")
                except Exception as e:
                    print("Error sending data to REST API:", e)
            time.sleep(5)  # 5초 간격으로 데이터 전송
    else:
        print("Failed to connect to MODBUS server")

# 실행
try:
    read_and_send()
except KeyboardInterrupt:
    print("Stopped by user")
finally:
    modbus_client.close()
```

### **필요한 라이브러리 설치**
```bash
pip install pymodbus requests
```

---

## **코드 설명**
### 공통 사항:
- **Modbus 데이터 읽기**: `read_holding_registers` 메소드를 사용하여 지정된 주소에서 데이터를 읽습니다.
- **주기적 실행**: `time.sleep(5)`를 사용해 매 5초마다 데이터를 읽고 전송합니다.

### MQTT 방식:
- `paho-mqtt` 라이브러리를 사용해 MQTT 브로커에 연결하고, 데이터를 특정 토픽으로 게시(`publish`)합니다.
- 브로커 주소와 포트를 정확히 설정해야 하며, 인증이 필요한 경우 추가 설정이 필요합니다.

### REST API 방식:
- `requests.post()` 메소드를 사용해 데이터를 JSON 형식으로 REST API 서버에 전송합니다.
- 서버의 엔드포인트 URL과 헤더를 정확히 설정해야 합니다.

---

## **주의사항**
1. **Modbus 연결 확인**: 장치의 IP 주소와 포트를 올바르게 설정하고 통신 상태를 점검하세요.
2. **MQTT/REST 서버 준비**: 브로커(MQTT) 또는 REST API 서버가 정상적으로 작동하는지 확인하세요.
3. **오류 처리**: 네트워크 오류나 서버 응답 실패에 대한 예외 처리를 추가적으로 구현하는 것이 좋습니다.
4. **보안**: MQTT 또는 REST API를 사용할 때 TLS/SSL과 같은 보안 프로토콜을 적용하여 데이터 보호를 강화하세요.

이 코드를 기반으로 필요에 따라 간격 조정 및 추가 기능을 구현할 수 있습니다!

출처
