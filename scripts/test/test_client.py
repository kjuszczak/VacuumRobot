import socket
import sys
import json

HOST, PORT = '127.0.0.1', 65425

data = '{' \
       '"x": 880,' \
       '"y": 880,' \
       '"angle": -45,' \
       '"add garbages": true,' \
       '"delete garbages": false,' \
       '"array of garbages":[' \
       '    {' \
       '        "x": 500,' \
       '        "y": 500' \
       '    },' \
       '    {' \
       '        "x": 400,' \
       '        "y": 400' \
       '    }' \
       ']' \
       '}'

data_flat = {
	"id": "flat",
	"length": 880,
	"width": 880,
	"walls": [
		{
			"start_x": 100,
			"start_y": 120,
			"end_x": 100,
			"end_y": 180
		},
        {
			"start_x": 200,
			"start_y": 180,
			"end_x": 360,
			"end_y": 180
		},
        {
			"start_x": 600,
			"start_y": 600,
			"end_x": 800,
			"end_y": 600
		}
	],
	"doors": [
		{
			"start_x": 680,
			"start_y": 600,
			"end_x": 760,
			"end_y": 600
		}
	],
	"furnitures": [
		{
			"furniture": [
				{
					"x": 150,
					"y": 150,
					"length": 200,
					"width": 50,
				},
                {
					"x": 300,
					"y": 150,
					"length": 50,
					"width": 80,
				}
			]
		}
	],
	"garbages": [
		{
			"x": 50,
			"y": 100
		},
        {
			"x": 70,
			"y": 100
		}
	]
}

data_flat = json.dumps(data_flat)

# data = json.loads(json_parameters)

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(bytes(data_flat,encoding="utf-8"))


    # Receive data from the server and shut down
    received = sock.recv(1024)
    received = received.decode("utf-8")

finally:
    sock.close()

print("Sent:     {}".format(data_flat))
print("Received: {}".format(received))