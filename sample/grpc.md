아래는 Python에서 gRPC를 사용하여 클라이언트와 서버 간에 데이터를 주고받는 간단한 예제입니다. 이 예제에서는 gRPC를 이용하여 소켓을 직접적으로 열지 않고, gRPC를 통한 통신을 구현합니다.

### 1. **Proto 파일 작성**
gRPC는 프로토콜 버퍼(Protocol Buffers)라는 인터페이스 정의 언어를 사용합니다. 먼저 `example.proto` 파일을 작성하여 gRPC 서비스와 메시지를 정의합니다.

```proto
syntax = "proto3";

service ExampleService {
    rpc SendMessage (MessageRequest) returns (MessageResponse);
}

message MessageRequest {
    string message = 1;
}

message MessageResponse {
    string response = 1;
}
```

### 2. **Proto 파일 컴파일**
이제 `protoc` 컴파일러를 사용하여 `.proto` 파일을 컴파일하여 Python 코드를 생성해야 합니다.

```bash
python -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. example.proto
```

위 명령어는 `example_pb2.py`와 `example_pb2_grpc.py`라는 파일을 생성합니다. 이 파일들은 gRPC 통신에 필요한 코드들을 포함합니다.

### 3. **gRPC 서버 코드**

서버는 클라이언트로부터 요청을 받아서 응답하는 역할을 합니다. 아래는 gRPC 서버를 구현한 예제입니다.

```python
import grpc
from concurrent import futures
import time

import example_pb2
import example_pb2_grpc

# gRPC 서버에서 동작할 클래스 정의
class ExampleServiceServicer(example_pb2_grpc.ExampleServiceServicer):
    def SendMessage(self, request, context):
        # 클라이언트로부터 받은 메시지 처리
        print(f"Received message from client: {request.message}")
        # 응답 메시지 생성
        return example_pb2.MessageResponse(response="Message received: " + request.message)

# gRPC 서버 실행
def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    example_pb2_grpc.add_ExampleServiceServicer_to_server(ExampleServiceServicer(), server)
    
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Server started, listening on port 50051.")
    
    try:
        while True:
            time.sleep(86400)  # 서버를 계속 실행하기 위해 sleep
    except KeyboardInterrupt:
        server.stop(0)

if __name__ == '__main__':
    serve()
```

### 4. **gRPC 클라이언트 코드**

클라이언트는 서버에 메시지를 보내고 응답을 받는 역할을 합니다.

```python
import grpc

import example_pb2
import example_pb2_grpc

def run():
    # 서버와 연결
    with grpc.insecure_channel('192.168.1.231:50051') as channel:
        stub = example_pb2_grpc.ExampleServiceStub(channel)
        
        # 서버로 메시지 보내기
        message = "Hello, gRPC server!"
        response = stub.SendMessage(example_pb2.MessageRequest(message=message))
        
        # 서버로부터 응답 받기
        print(f"Received from server: {response.response}")

if __name__ == '__main__':
    run()
```

### 5. **실행 방법**

#### 서버 실행:
서버를 실행하여 클라이언트의 요청을 대기합니다.
```bash
python server.py
```

#### 클라이언트 실행:
클라이언트를 실행하여 서버로 메시지를 보내고 응답을 받습니다.
```bash
python client.py
```

### 요약
1. **`example.proto`**: gRPC 인터페이스를 정의하는 파일.
2. **서버 코드**: 클라이언트로부터 메시지를 받아서 처리하고 응답을 보내는 서버.
3. **클라이언트 코드**: 서버에 메시지를 보내고 응답을 받는 클라이언트.

이 예제는 Python에서 gRPC를 이용한 간단한 클라이언트-서버 통신을 보여줍니다.
