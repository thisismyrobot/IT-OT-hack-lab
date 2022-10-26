import random
import time

import pymodbus.client


# Using CLICK Hex Addressing.
PUMP_ENABLE_VALUE = 0x0000
PUMP_ENABLE_FLAG_VALUE = 0x0001

VALVE_CTRL_ENABLE_VALUE = 0x0002
VALVE_CTRL_ENABLE_FLAG_VALUE = 0x0003

VALVE_OPEN = 0x2021

PRESSURE_SIM = []


def get_flags(client):
    return (
        client.read_holding_registers(PUMP_ENABLE_FLAG_VALUE).registers[0],
        client.read_holding_registers(VALVE_CTRL_ENABLE_FLAG_VALUE).registers[0],
    )


def pump_on(client, pump_flag):
    client.write_register(PUMP_ENABLE_VALUE, pump_flag)


def valve_ctrl_enable(client, valve_flag):
    client.write_register(VALVE_CTRL_ENABLE_VALUE, valve_flag)


def high_pressure_reached(client):
    return len(PRESSURE_SIM) > 8


def low_pressure_reached(client):
    return len(PRESSURE_SIM) < 4


def valve_open(client):
    client.write_coil(VALVE_OPEN, True)


def test(ip, port=502):
    global PRESSURE_SIM
    draining = False

    client = pymodbus.client.ModbusTcpClient(ip, port=port)

    # Read bootstrap state.
    pump_flag, valve_flag = get_flags(client)

    # Operate.
    while True:
        valve_ctrl_enable(client, valve_flag)

        if high_pressure_reached(client) or draining:
            draining = True
            valve_open(client)
            PRESSURE_SIM = PRESSURE_SIM[:-2]

            if low_pressure_reached(client):
                draining = False
        else:
            pump_on(client, pump_flag)
            PRESSURE_SIM.append('.')

        print(''.join(PRESSURE_SIM))

        time.sleep(4)

    client.close()


if __name__ == '__main__':
    test('192.168.0.10')
