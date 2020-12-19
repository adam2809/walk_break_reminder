try:
    import usocket as socket
except:
    import socket
import network
import esp
import aphttpserver
import gc

esp.osdebug(None)
gc.collect()

SERVER_NAME = "ESP32 Strava Tracker"
CLIENT_ID = ''
SSID = 'MicroPython-AP'
PASSWORD = '123456789'
PORT = 80

esp.osdebug(None)

gc.collect()

ap_server = aphttpserver.APHttpServer(
    SERVER_NAME,
    SSID,
    PASSWORD,
    PORT
)

ap_server.loop();
