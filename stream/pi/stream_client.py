import asyncio
import json
import queue
import sys
import datetime

import pyshark.tshark.tshark
import websockets


EXCLUSIONS = [
    ('_index',),
    ('_type',),
    ('_score',),
    ('_source', 'layers', 'frame', 'frame.interface_id'),
    ('_source', 'layers', 'frame', 'frame.interface_id_tree'),
    ('_source', 'layers', 'frame', 'frame.encap_type'),
    ('_source', 'layers', 'frame', 'frame.time'),
    ('_source', 'layers', 'frame', 'frame.offset_shift'),
    ('_source', 'layers', 'frame', 'frame.time_epoch'),
    ('_source', 'layers', 'frame', 'frame.time_delta'),
    ('_source', 'layers', 'frame', 'frame.time_delta_displayed'),
    ('_source', 'layers', 'frame', 'frame.time_relative'),
    ('_source', 'layers', 'frame', 'frame.number'),
    ('_source', 'layers', 'frame', 'frame.len'),
    ('_source', 'layers', 'frame', 'frame.cap_len'),
    ('_source', 'layers', 'frame', 'frame.marked'),
    ('_source', 'layers', 'frame', 'frame.ignored'),
    ('_source', 'layers', 'eth'),
    ('_source', 'layers', 'ip', 'ip.version'),
    ('_source', 'layers', 'ip', 'ip.hdr_len'),
    ('_source', 'layers', 'ip', 'ip.dsfield'),
    ('_source', 'layers', 'ip', 'ip.dsfield_tree'),
    ('_source', 'layers', 'ip', 'ip.len'),
    ('_source', 'layers', 'ip', 'ip.id'),
    ('_source', 'layers', 'ip', 'ip.flags'),
    ('_source', 'layers', 'ip', 'ip.flags_tree'),
    ('_source', 'layers', 'ip', 'ip.frag_offset'),
    ('_source', 'layers', 'ip', 'ip.ttl'),
    ('_source', 'layers', 'ip', 'ip.ttl_tree'),
    ('_source', 'layers', 'ip', 'ip.checksum'),
    ('_source', 'layers', 'ip', 'ip.checksum.status'),
    ('_source', 'layers', 'tcp', 'tcp.completeness'),
    ('_source', 'layers', 'tcp', 'tcp.stream'),
    ('_source', 'layers', 'tcp', 'tcp.len'),
    ('_source', 'layers', 'tcp', 'tcp.seq'),
    ('_source', 'layers', 'tcp', 'tcp.seq_raw'),
    ('_source', 'layers', 'tcp', 'tcp.nxtseq'),
    ('_source', 'layers', 'tcp', 'tcp.seq'),
    ('_source', 'layers', 'tcp', 'tcp.ack'),
    ('_source', 'layers', 'tcp', 'tcp.ack_raw'),
    ('_source', 'layers', 'tcp', 'tcp.hdr_len'),
    ('_source', 'layers', 'tcp', 'tcp.options_tree'),
    ('_source', 'layers', 'tcp', 'tcp.options.mss_tree'),
    ('_source', 'layers', 'tcp', 'tcp.flags'),
    ('_source', 'layers', 'tcp', 'tcp.flags_tree'),
    ('_source', 'layers', 'tcp', 'tcp.window_size_value'),
    ('_source', 'layers', 'tcp', 'tcp.window_size'),
    ('_source', 'layers', 'tcp', 'tcp.window_size_scalefactor'),
    ('_source', 'layers', 'tcp', 'tcp.checksum'),
    ('_source', 'layers', 'tcp', 'tcp.checksum.status'),
    ('_source', 'layers', 'tcp', 'tcp.urgent_pointer'),
    ('_source', 'layers', 'tcp', 'Timestamps'),
    ('_source', 'layers', 'tcp', 'tcp.analysis'),
    ('_source', 'layers', 'udp', 'Timestamps'),
    ('_source', 'layers', 'udp', 'udp.length'),
    ('_source', 'layers', 'udp', 'udp.checksum'),
    ('_source', 'layers', 'udp', 'udp.checksum.status'),
    ('_source', 'layers', 'udp', 'udp.stream'),
    ('_source', 'layers', 'tls'),
]


def downsize(packet):
    for exclusion in EXCLUSIONS:
        pointer = packet
        for layer in exclusion[:-1]:
            if pointer is None:
                break
            pointer = pointer.get(layer)
        if pointer is not None:
            pointer.pop(exclusion[-1], None)
    return packet['_source']['layers']


async def pcap_client(interface, server_and_port):
    async with websockets.connect(f'ws://{server_and_port}/packets/feed') as websocket:
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

        packet = None
        while True:
            line = await proc.stdout.readline()
            line = line.decode()
            if line.startswith('['):
                continue
            elif line.startswith('  {'):
                if packet is not None:
                    # Trim trailing comma
                    packet = packet.strip()[:-1]
                    obj = json.loads(packet)
                    payload = json.dumps(downsize(obj))
                    await websocket.send(payload)
                packet = line
            else:
                packet += line


def main(interface, server_and_port):
    asyncio.run(pcap_client(interface, server_and_port))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
