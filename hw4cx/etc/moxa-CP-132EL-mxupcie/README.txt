1. Get the driver from
https://cdn-cms.azureedge.net/getmedia/9fa12c30-9bad-4d77-ad43-7810f74b8410/moxa-linux-kernel-5.x.x-driver-v5.1.tgz

2. Unpack it and build the driver:
	cd mxser/driver/kernel5.x/
	make

3. Install the mxupcie.ko module (info got from "make -n install"):
	cp -p mxupcie.ko /lib/modules/5.10.0-18-amd64/kernel/drivers/tty/
	chown 0.0 /lib/modules/5.10.0-18-amd64/kernel/drivers/tty/mxupcie.ko
	depmod -a

4. Setup the "moxa_unbind" service
	cd mxser/driver
	mkdir /etc/moxa
	cp -p moxa_unbind /etc/moxa
	chown 0.0 /etc/moxa/moxa_unbind
	cp -p moxa_unbind.service /usr/lib/systemd/system
	chown 0.0 /usr/lib/systemd/system/moxa_unbind.service
	systemctl enable moxa_unbind
