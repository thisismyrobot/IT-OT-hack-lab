# Binaries as part of the challenge, compilable for the Moxa IA240-LX

Based on the support disk that came with the Moxa IA240-LX.

Remember:

    export PATH="/usr/local/arm-linux/bin:$PATH"

And to make and deploy use something like:

    make && curl -v -T "controller" -u root:root ftp://192.168.3.127//tmp/
