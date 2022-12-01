# HMI

A simple *read-only* HMI for the SCADA system.

## Deploy

The contents of `www` go in `/home/httpd/htdocs/` on the Moxa.

TODO: Where to run the supervisor binary from?

## How it works (this is the plan at least...)

A new process will read data from the PLC over Modbus, writing out a JSON (or
other suitable format) file to ramfs. This will be fast and prevents
write-wear of the storage.

A static website hosted in the Moxa will include a symlink to that file, which
can then be loaded via JavaScript to guide the presentation current state of
the system.

## To confirm

### Does the Moxa support ramfs?

If not I can use the USB because I'm not precious about that. It would be nice
though as the flash is NOR flash, and that doesn't wear on read (of the
symlink) [ref](https://electronics.stackexchange.com/a/429974).

Yes, according to `moxa-ia240-series-linux-manual-v6.0.pdf`.

```bash
upramdisk
cd /mnt/ramdisk
```

### Can I symlink from a hosted website to a file in ramfs?

Symlink needs to be from `/home/httpd/htdocs/`. And `ln` is definitely
implemented in the Moxa. Maybe something like this:

```bash
upramdisk
echo '{}' > /mnt/ramdisk db.json
ln -s /mnt/ramdisk/db.json /home/httpd/htdocs/db.json
```

### Can I access the symlink'd file from the browser?

Considering the above, I see no reason not.
