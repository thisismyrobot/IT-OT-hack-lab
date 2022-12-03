import queue

from flask import Flask, render_template
from flask_sock import Sock


monitor_queue = queue.Queue()

app = Flask(__name__)
sock = Sock(app)


@sock.route('/packets/feed')
def feed(ws):
    """Receives pcap data from the Pi monitoring the hack lab.

    TODO: Auth...
    """
    while True:
        data = ws.receive()

        # Prevent blowing up the queue if the consumer stops.
        if monitor_queue.qsize() < 10:
            monitor_queue.put(data)


@sock.route('/packets/consume')
def consume(ws):
    """Provides queued pcap data to a web browser client."""
    while True:
        line = monitor_queue.get()
        ws.send(line)


@app.route('/')
def index():
    return render_template('index.html')
