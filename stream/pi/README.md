# Streamer Pi client

Pushes video and pcap up to the web server via websockets, from a Raspberry
Pi.

## Install

```bash
sudo apt-get install tshark
python -m pip install pipenv
pipenv install
```

## Run

```bash
pipenv run python stream_client.py [interface] [server hostname]
```
