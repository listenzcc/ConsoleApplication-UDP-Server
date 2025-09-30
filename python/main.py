"""
File: main.py
Author: Chuncheng Zhang
Date: 2025-09-23
Copyright & Email: chuncheng.zhang@ia.ac.cn

Purpose:
    Send events through UDP socket.
    The keyboard events are captured by the `keyboard` library.
    And the keyboard events are sent in the format of: ${event_type}:{name}:{scan_code} [timestamp]
    The messages are sent to localhost:12345 by default.

Functions:
    1. Requirements and constants
    2. Function and class
    3. Play ground
    4. Pending
    5. Pending
"""


# %% ---- 2025-09-23 ------------------------
# Requirements and constants
import time
import socket
import loguru
import keyboard
from datetime import datetime

# %%
host = 'localhost'
port = 5174

logger = loguru.logger


# %% ---- 2025-09-23 ------------------------
# Function and class

class MyUDPClient:
    ''' A simple UDP client to send messages to a UDP server. '''
    # Make a UDP client
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = (host, port)

    def send(self, message=b'Hello, UDP Server!'):
        try:
            message = message if isinstance(
                message, bytes) else message.encode()
            logger.info(f"Sent: {message.decode()}")
            self.sock.sendto(message, self.server_address)
        except Exception as e:
            logger.exception(e)


my_udp_client = MyUDPClient()


def on_key_event(event):
    key_info = {
        'event_type': event.event_type,  # 'down' 或 'up'
        'name': event.name,
        'scan_code': event.scan_code,
        'time': event.time
    }

    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]  # 保留3位毫秒

    # 仅处理按键按下事件
    if not key_info['event_type'] == 'down':
        return

    # 发送按键信息
    message = f"KEY:{key_info['event_type']}:{key_info['name']}:{key_info['scan_code']} [{now}]"

    import random
    import time
    evts = ['1', '2']
    random.shuffle(evts)
    message = f'{evts[0]},{int(time.time())}'

    my_udp_client.send(message)
    return


keyboard.hook(on_key_event)

# %% ---- 2025-09-23 ------------------------
# Play ground
if __name__ == "__main__":
    my_udp_client.send()

    import random
    for _ in range(5):
        my_udp_client.send(f"Message {_ + 1}")
        time.sleep(random.random())

    input("Press Enter to stop...\n")


# %% ---- 2025-09-23 ------------------------
# Pending


# %% ---- 2025-09-23 ------------------------
# Pending
