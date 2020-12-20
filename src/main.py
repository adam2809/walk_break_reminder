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

wlan = network.WLAN(network.STA_IF)
wlan.active(True)


# ap_server = aphttpserver.APHttpServer(
#     SERVER_NAME,
#     SSID,
#     PASSWORD,
#     PORT
# )
#
# ap_server.add_endpoint('/ok',lambda req : aphttpserver.Response(
#     200,
#     'okejka'
# ))
# ap_server.add_endpoint('/',lambda req : aphttpserver.Response(
#     200,
#     'home'
# ))
#
# ap_server.loop();
