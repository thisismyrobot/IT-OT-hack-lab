# Automation controller for the PLC

Instructs the PLC to operate the pump and valve in a cycle.

## Modbus commands

### Run pump

Register address: 0x0000
Value: [Match flag]

### Enable valve control

Register address: 0x0002
Value: [Match flag]

Must be sent every [x] seconds, or pump and valve control is locked out.

### Request open valve

Coil address: 0x2021
Value: True or False

## Flags

### Enable pump

Register address: 0x0001
Value: [Random]

### Enable valve control

Register address: 0x0003
Value: [Random]
