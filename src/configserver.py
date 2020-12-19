# Complete project details at https://RandomNerdTutorials.com

SERVER_NAME = "ESP32 Strava Tracker"
CLIENT_ID = ''

try:
    import usocket as socket
except:
    import socket
import network
import esp

esp.osdebug(None)

import gc
gc.collect()

SSID = 'MicroPython-AP'
PASSWORD = '123456789'

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid=SSID, password=PASSWORD)

while ap.active() == False:
    pass

print('Connection successful')
print(ap.ifconfig())

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)


while True:
    conn, addr = s.accept()
    print('Got a connection from %s' % str(addr))
    request = conn.recv(1024)
    print('Content = %s' % str(request))

    redirect_url = 'http://www.strava.com/oauth/authorize? \
client_id=%s& \
response_type=code& \
redirect_uri=http://localhost/exchange_token \
&approval_prompt=force& \
scope=read' % CLIENT_ID

    response = '\r\n'.join([
        'HTTP/1.1 303 Permanent Redirect',
        'Content-Length: 0',
        'Location: %s' % redirect_url,
        ''
    ])

    print('Sending = %s' % response)
    conn.send(response)
    conn.close()
