try:
    import usocket as socket
except:
    import socket
import network
import esp
import gc

ap = network.WLAN(network.AP_IF)

class APHttpServer:
    def __init__(
        self,
        server_name,
        ssid,
        password,
        port
    ):
        self.server_name = server_name
        self.ssid = ssid
        self.password = password
        self.port = port

        self.activate_ap()
        self.bind_socket();


    def activate_ap(self):
        print('Trying to activate')

        ap.active(True)
        ap.config(essid=self.ssid, password=self.password)

        while ap.active() == False:
            pass

        print('Activation successful')
        print(ap.ifconfig())

    def bind_socket(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind(('', 80))
        self.s.listen(5)

    def loop(self):
        while True:
            conn, addr = self.s.accept()
            print('Got a connection from %s' % str(addr))
            request = conn.recv(1024)
            print('Content = %s' % str(request))

            response = '\r\n'.join([
                'HTTP/1.1 200 OK',
                'Content-Length: 6',
                '',
                'okejka'
            ])

            print('Sending = %s' % response)
            conn.send(response)
            conn.close()
