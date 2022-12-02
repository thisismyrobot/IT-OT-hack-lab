from flask import Flask, render_template
from flask_sock import Sock


app = Flask(__name__)
sock = Sock(app)


@sock.route('/monitor')
def echo(ws):
    while True:
        data = ws.receive()
        print(data)
