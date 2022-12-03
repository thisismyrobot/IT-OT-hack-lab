import queue

from flask import Flask, render_template
from flask_sock import Sock


monitor_queue = queue.Queue()

app = Flask(__name__)
sock = Sock(app)


@sock.route('/packets/feed')
def feed(ws):
    while True:
        data = ws.receive()
        if monitor_queue.qsize() < 10:
            monitor_queue.put(data)


@sock.route('/packets/consume')
def consume(ws):
    while True:
        line = monitor_queue.get()
        ws.send(line)


@app.route('/')
def index():
    return render_template('index.html')
