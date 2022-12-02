import asyncio
import queue
import sys

import pyshark.tshark.tshark
import websockets



async def pcap_client(interface, server, port=8765):
    async with websockets.connect(f'ws://{server}:8765') as websocket:
        cmd = [
            pyshark.tshark.tshark.get_process_path(),
            '-i', interface,
            '-T' 'json'
        ]

        proc = await asyncio.create_subprocess_exec(
            *cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )

        while True:
            line = await proc.stdout.readline()
            await websocket.send(line)


def main(interface, server):
    asyncio.run(pcap_client(interface, server))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
