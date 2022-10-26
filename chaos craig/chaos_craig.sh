#!/bin/bash

# Chaos Craig
#
# Sets up the hack lab with some random elements.
#
# Run (from Kali):
#
#     ./chaos_craig.sh
#
# Designed to be ran when the after the Moxa has been reset (or at least
# rebooted with the root password back to 'root').
#

# Set up top-level randomised configuration for this chaos run.
new_moxa_root_password="root-$(cat /dev/random | head -c 10 | md5sum | head -c 3)"
new_test_cgi=`shuf -n 1 dirb_wordlist_subset.txt`

# Save critical recovery info (just means we don't need to hard-reset the
# Moxa), and/or we can recover from a failure during the config.
echo "Moxa Root password: $new_moxa_root_password" > session.txt

# Set up the Moxa.
curl -v -T "configure_moxa.sh" -u root:root ftp://192.168.3.127//tmp/
expect run_moxa_configuration.expect $new_moxa_root_password $new_test_cgi
