# IT/OT Hack lab

A sandboxed IT/OT hack lab/cyber range with the disruption of a physical
automation as the target/goal.

This project is currently under heavy redevelopment and will change regularly,
but the current state of [the physical build](factory/) is:

![](factory/hardware.png)

The components are:

 - Moxa IA240-LX with old (and vulnerable) firmware
 - NETGEAR ProSAFE GS105 Gigabit switch
 - Koyo CLICK C0-11DRE-D PLC
 - A (WIP) [model factory automation](factory/)

## Important note

This project is for my own learning and enjoyment as well as to support
capture the flag events with friends. It obviously relies on a real world
automation so isn't a simple clone-and-build repository. As such, please
understand I won't be providing support for what is in here, I'm simply
sharing it to benefit anyone who might find it interesting or helpful for
their project.

## The scenario

You goal is blow up a factory!

This is what we know:

 * There's a Moxa IA240-LX on the 192.168.0.0/16 network, with a
   Shellshock-vulnerable Apache instance.
 * There's a PLC on the network at 192.168.0.10.
 * The Moxa is supervising the a PLC in the control of the factory.

### Reset

Reset the Moxa to factory settings, or at least reset the Moxa root password
to 'root'.

Run the `chaos_craig.sh` script to set up the random elements and start the
factory.

## Thank you

This project wouldn't have been possible without the following people's
advice, patience, laser cutting and spare parts:

 - [Simon Riley](https://www.linkedin.com/in/simon-riley-a3679b84)
 - [Michael Cullen](https://www.linkedin.com/in/michael-cullen-45155b66)
