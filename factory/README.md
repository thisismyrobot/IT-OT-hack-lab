# Factory (WIP)

The factory automation is built into the same physical hardware as the PLC and
other hardware.

This is very very WIP, expect it to change often.

The automation is based off a fill and drain cycle using a pump and a valve.
The cycle fills and drains from a cylinder, and (to keep it all safe in a
corporate office) the pressure is represented by a balloon that fills up
inside the tank. A pressure switch detects the balloon nearly completely
filling tank and triggers the valve to empty the system. Going beyond the
pressure that triggers the switch brings the balloon in contact with a small
spike, popping it, which irrefutably signals that the goal has been reached.

The Moxa runs everything, acting as a pseudo-SCADA system, with the PLC doing
the actual actioning of commands. The PLC has built-in interlocks to prevent
simple overriding of the motor and valve controls, so causing the system to
malfunction such that the balloon pops should be a non-trivial exercise, even
if you have access to all the code and information about the system.

## Current state of the real build

![](hardware.png)

## Current target design

![](model_factory.png)

## Animation of a semi-current version of the design

![](model_factory.gif)
