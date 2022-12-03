import queue

from flask import Flask, render_template
from flask_sock import Sock

import simple_websocket.ws


monitor_queues = {}

app = Flask(__name__)
sock = Sock(app)


@sock.route('/packets/feed')
def feed(ws):
    """Receives pcap data from the Pi monitoring the hack lab.

    TODO: Auth...
    """
    while True:
        data = ws.receive()

        for monitor_queue in monitor_queues.values():
            # Prevent blowing up the queue if the consumer stops.
            if monitor_queue.qsize() < 100:
                monitor_queue.put(data)


@sock.route('/packets/consume')
def consume(ws):
    """Provides queued pcap data to a web browser client."""
    fileno = ws.sock.fileno()
    try:
        ws_queue = monitor_queues[fileno]
    except KeyError:
        ws_queue = monitor_queues[fileno] = queue.Queue()
    while True:
        line = ws_queue.get()

        try:
            ws.send(line)
        except simple_websocket.ws.ConnectionClosed:
            del monitor_queues[fileno]


@app.route('/')
def index():
    return render_template('index.html')
