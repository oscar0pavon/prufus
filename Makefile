
.PHONY: prufus

prufus:
	make -C source_code

install:
	cp prufus /usr/bin/
	cp ./scripts/prufus.sh /usr/bin/
	mkdir -p /usr/libexec/prufus
	cp ./scripts/get_usb_disks.sh /usr/libexec/prufus/
	cp ./scripts/graphics_sudo /usr/libexec/prufus/
	mkdir -p /usr/share/icons/prufus
	cp ./icon/icon.png /usr/share/icons/prufus/prufus.png
	cp ./prufus.desktop /usr/share/applications
	cp ./prufus.desktop /home/${SUDO_USER}/Desktop/
	chmod 777 /home/${SUDO_USER}/Desktop/prufus.desktop

uninstall:
	rm -rf /usr/libexec/prufus
	rm -f /usr/bin/prufus
	rm -f /usr/bin/prufus.sh
	rm -rf /usr/share/icons/prufus
	rm -f /usr/share/applications/prufus.desktop
	rm -f /home/${SUDO_USER}/Desktop/prufus.desktop

clean:
	make -C source_code clean
	rm -f prufus
