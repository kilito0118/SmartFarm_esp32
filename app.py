from flask import Flask, render_template
from flask_sock import Sock
import socket
import json
import threading
import time

app = Flask(__name__)
sock = Sock(app)

clients = set()
last_data_from_esp32 = None
esp32_connected = False

# 주기적으로 브라우저로 데이터 전송 (2초 간격)
def broadcast_to_clients():
    global last_data_from_esp32, esp32_connected

    while True:
        if esp32_connected and last_data_from_esp32:
            payload = last_data_from_esp32
        else:
            # 브라우저에서 연결되었지만 ESP32 없을 때는 빈 JSON 혹은 null 값을 보냄
            payload = json.dumps({"esp32": False})

        for client in clients.copy():
            try:
                client.send(payload)
            except Exception as e:
                print("브라우저 전송 오류:", e)
                clients.discard(client)

        time.sleep(2)

# 👉 ESP32에서 받은 WebSocket 데이터 처리
@sock.route('/ws/esp')
def esp32_ws(ws):
    global last_data_from_esp32, esp32_connected
    print("[ESP32 연결됨]")
    esp32_connected = True
    try:
        while True:
            msg = ws.receive()
            if msg is None:
                break
            print("[ESP32 데이터 수신]:", msg)
            last_data_from_esp32 = msg
    finally:
        print("[ESP32 연결 종료]")
        esp32_connected = False
        last_data_from_esp32 = None

# 👉 브라우저로부터 수신용 WebSocket
@sock.route('/ws/client')
def client_ws(ws):
    print("[브라우저 연결됨]")
    clients.add(ws)
    try:
        while True:
            msg = ws.receive()
            if msg is None:
                break
            # 메시지를 처리할 게 있다면 이곳에 작성
    finally:
        print("[브라우저 연결 종료]")
        clients.discard(ws)

@app.route('/')
def index():
    return render_template("dashboard.html")

def get_ip():
    import socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]
    finally:
        s.close()

if __name__ == '__main__':
    host_ip = get_ip()
    print("="*60)
    print(f"Flask 실행: http://{host_ip}:5000")
    print(f"WebSocket (ESP32): ws://{host_ip}:5000/ws/esp")
    print(f"WebSocket (Client): ws://{host_ip}:5000/ws/client")
    print("="*60)

    # 브라우저로 데이터 뿌려주는 스레드 시작
    thread = threading.Thread(target=broadcast_to_clients)
    thread.daemon = True
    thread.start()

    app.run(host='0.0.0.0', port=5000)
