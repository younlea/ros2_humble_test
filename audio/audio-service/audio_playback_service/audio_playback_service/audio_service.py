import os

# import sys
# sys.path.append('/home/nvidia/source-code/audio-service/audio_playback_service')

import rclpy
from rclpy.node import Node
from simpleaudio import WaveObject, stop_all
from threading import Thread, Lock

from audio_msgs.srv import AudioControl

class AudioServer(Node):
    def __init__(self):
        super().__init__('audio_server')
        self.service = self.create_service(AudioControl, 'audio_control', self.handle_request)
        self.current_track = None  # 현재 재생 중인 트랙 번호 (None이면 재생 중 아님)
        self.volume = 1.0          # 초기 볼륨 값 (100%)
        self.lock = Lock()         # 동시 접근 방지용 Lock 객체

        # wav 디렉토리 설정 (패키지 내 위치)
        self.wav_dir = os.path.join(os.path.dirname(__file__), '..', '../../..', 'share','audio_playback_service','wav')
        self.get_logger().info(f'Audio Server initialized. Wav directory: {self.wav_dir}')

    def handle_request(self, request, response):
        with self.lock:
            if request.command == "play":
                if self.current_track is not None:
                    response.success = False
                    response.message = "이미 재생 중인 트랙이 있습니다."
                    return response

                if request.track not in [1, 2, 3]:
                    response.success = False
                    response.message = f"잘못된 트랙 번호: {request.track}"
                    return response

                self.play_track(request.track)
                response.success = True
                response.message = f"트랙 {request.track} 재생 시작"

            elif request.command == "set_volume":
                if not (0.0 <= request.volume <= 1.0):
                    response.success = False
                    response.message = f"잘못된 볼륨 값: {request.volume}"
                    return response

                self.volume = request.volume
                response.success = True
                response.message = f"볼륨 {self.volume * 100}%로 설정"

            else:
                response.success = False
                response.message = "잘못된 명령어"

            return response

    def play_track(self, track_number):
        try:
            print("play")
            # 음원 파일 경로 생성 (예: wav/1_track.wav)
            filename = os.path.join(self.wav_dir, f"{track_number}_track.wav")
            
            if not os.path.exists(filename):
                self.get_logger().error(f"파일 없음: {filename}")
                return

            wave_obj = WaveObject.from_wave_file(filename)
            play_obj = wave_obj.play()
            self.current_track = track_number
            
            # 비동기적으로 재생 완료 대기 후 상태 초기화
            def wait_and_reset():
                play_obj.wait_done()
                self.current_track = None
                
            Thread(target=wait_and_reset).start()

        except Exception as e:
            self.get_logger().error(f"재생 오류: {str(e)}")
            self.current_track = None

def main(args=None):
    rclpy.init(args=args)
    node = AudioServer()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()