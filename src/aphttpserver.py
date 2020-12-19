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

        self.endpoint_action_mapping = dict()

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


    def add_endpoint(self,path,action):
        self.endpoint_action_mapping[path] = action


    def loop(self):
        while True:
            conn, addr = self.s.accept()
            print('Got a connection from %s' % str(addr))
            request = conn.recv(1024)
            print('Content = %s' % str(request))

            (method,path) = split_request(request)

            response = self.endpoint_action_mapping[path](request)

            response_str = make_response_str(response)

            print('Sending = %s' % response_str)
            conn.send(response_str)
            conn.close()


def split_request(request):
    request_line = request.decode("utf-8").split('\n')[0]

    return (request_line.split(' ')[0],request_line.split(' ')[1])


def make_response_str(response):
    return '\r\n'.join([
        'HTTP/1.1 %s' % response.get_status_string(),
        'Content-Length: %s' % response.content_length,
        'Content-type: application/json',
        '',
        response.content
    ])


class Response:
    STATUS_NAME_DICT = {
        200:'OK',
        404:'Not Found'
    }

    def __init__(self,status,content):
        self.status = status
        self.content = content
        self.content_length = len(content)

    def get_status_string(self):
        return '%s %s' % (self.status, self.STATUS_NAME_DICT[self.status])
