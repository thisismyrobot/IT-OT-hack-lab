# Chaos Craig

A set of scripts to configure the hack lab before having a go at it.

Run `chaos_craig.sh` from a Kali image that can access a freshly rebooted Pi
Moxa device with the password set back to default.

 This:

 - Randomises the Moxa IP (192.168.0.0/16)
 - Changes the Moxa cgi-bin script name to one of a random set
 - Adds a guest account to the Moxa
 - Randomises the Moxa root password
 - Sets up controller binary to only work as root or nobody, and starts it
