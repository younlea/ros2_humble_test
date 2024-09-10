import socket

# 서버의 IP와 포트 설정
SERVER_IP = '192.168.1.231'  # 목적지 IP 주소
SERVER_PORT = 5002           # 목적지 포트

# 소켓 생성 (AF_INET: IPv4, SOCK_STREAM: TCP)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # 서버에 연결 시도
    client_socket.connect((SERVER_IP, SERVER_PORT))
    print(f'Connected to {SERVER_IP}:{SERVER_PORT}')
    
    # 전송할 메시지
    message = "Hello, this is a test message."
    
    # 메시지를 서버로 전송
    client_socket.sendall(message.encode())  # 문자열을 바이트로 인코딩하여 전송
    print(f'Sent: {message}')
    
    # 서버로부터 응답 수신 (최대 1024 바이트)
    response = client_socket.recv(1024)
    print(f'Received from server: {response.decode()}')

finally:
    # 소켓을 닫습니다.
    client_socket.close()
    print("Connection closed.")
