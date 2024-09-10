import socket

# 서버 IP와 포트 설정
HOST = '0.0.0.0'  # 모든 IP에서 들어오는 연결을 허용
PORT = 50051      # 포트 번호

# 소켓 생성 (AF_INET: IPv4, SOCK_STREAM: TCP)
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 주소 재사용 옵션 설정
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# 서버 소켓을 IP와 포트에 바인딩
server_socket.bind((HOST, PORT))

# 연결 대기 (최대 5개의 클라이언트 대기)
server_socket.listen(5)
print(f'Server listening on {HOST}:{PORT}...')

try:
    while True:
        # 클라이언트 연결 수락
        client_socket, addr = server_socket.accept()
        print(f'Connected by {addr}')

        # 데이터 수신
        while True:
            data = client_socket.recv(1024)  # 1024 바이트씩 데이터 수신
            if not data:
                break  # 더 이상 데이터가 없으면 루프 종료
            print(f'Received from {addr}: {data.decode()}')

        # 클라이언트 소켓 닫기
        client_socket.close()
        print(f'Connection closed with {addr}')

except KeyboardInterrupt:
    print("Server is shutting down...")

finally:
    # 서버 소켓 닫기
    server_socket.close()
