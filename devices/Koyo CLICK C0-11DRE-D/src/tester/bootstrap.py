import random
import time

import pymodbus.client


# Using CLICK Hex Addressing.
PUMP_ENABLE_FLAG_VALUE = 0x0001
VALVE_CTRL_ENABLE_FLAG_VALUE = 0x0003


def set_flags(client, pump_flag, valve_ctrl_flag):
    client.write_register(PUMP_ENABLE_FLAG_VALUE, pump_flag)
    client.write_register(VALVE_CTRL_ENABLE_FLAG_VALUE, valve_ctrl_flag)


def test(ip, port=502):
    client = pymodbus.client.ModbusTcpClient(ip, port=port)

    # Bootstrap.
    pump_flag = random.randint(10, 200)
    valve_flag = random.randint(10, 200)
    set_flags(client, pump_flag, valve_flag)

    client.close()


if __name__ == '__main__':
    test('192.168.0.10')
