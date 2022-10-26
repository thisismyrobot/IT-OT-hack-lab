#!/bin/bash

# Set up the controller binary. The chown makes sure root and the nobody can
# access it, but not the guest account you can use to access via telnet.
# Nobody is the account that the web server runs under.
cp /mnt/usbstorage/controller /tmp/
chown nobody /tmp/controller
chmod o-x /tmp/controller
umount /mnt/usbstorage

# Rename the vulnerable cgi-bin script so it's not so easy to guess.
new_test_cgi=$1
cd /home/httpd/cgi-bin/
mv * test-cgi
mv test-cgi $new_test_cgi

# Disable guest access to shell unless via Serial Console.
echo "if [ \${LOGNAME} = 'guest' ] && [ \${PPID} != '1' ]; then echo \"That would be too easy...\"; exit 0; fi" >> /etc/profile

# Re-configure the last two octets of the Ethernet interface.
new_ip=`perl -e "print '192.168.' . int(rand(200)+50) . '.' . int(rand(200)+50)"`
ifconfig eth0 $new_ip netmask 255.255.0.0

# Start the controller
cd /tmp/
./controller &
