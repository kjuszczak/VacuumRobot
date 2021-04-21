#!/usr/bin/python

import threading
import socket
import json
from Flat import *


# Initialize TCP parameters
HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65424        # Port to listen on (non-privileged ports are > 1023)


# Create TCP thread class
class TCPThread (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name

    def run(self):
        # run socket
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.bind(('', 1100))
            while True:
                data, address = s.recvfrom(1024)
                if not data:
                    continue
                json_parameters = json.loads(data.decode("utf-8"))
                id_v = json_parameters['id']
                if id_v == 'flat':
                    threadLock.acquire()
                    flat.createFlat(json_parameters)
                    threadLock.release()
                elif id_v == 'robot':
                    threadLock.acquire()
                    flat.updateParameters(json_parameters)
                    threadLock.release()
                # text = 'Siema'
                # conn.sendall(bytes(text, encoding="utf-8"))


# Create Visualization Thread class
class VisThread (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name

    def run(self):
        while True:
            # Get lock to synchronize threads
            threadLock.acquire()
            flat.draw(surface)
            # Free lock to release next thread
            threadLock.release()
            pygame.display.update()
            pygame.time.delay(10)


# Initialize game engine
pygame.init()

# Initialize clock
clock = pygame.time.Clock()

# Initialize surface
surface = pygame.display.set_mode([640, 480])

# Create flat
flat = Flat(640, 480)

# Create thread lock
threadLock = threading.Lock()
threads = []

# Create new threads
tcpThread = TCPThread(1, "TCP-Thread")
visThread = VisThread(2, "Vis-Thread")

# Start new Threads
tcpThread.start()
visThread.start()

# Add threads to thread list
threads.append(tcpThread)
threads.append(visThread)

# Wait for all threads to complete
for t in threads:
    t.join()
print("Exiting Main Thread")
pygame.display.quit()