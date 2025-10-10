CFLAGS := -I/usr/include
CLINKS := -lX11 -lGL -lpthread -lm

.PHONY: prufus

prufus: main.c
	cc $(CFLAGS) main.c -o ../prufus $(CLINKS)

user_interface.o: user_interface.c
	cc $(CFLAGS) -c user_interface.c

usb_validator.o: usb_validator.c
	cc $(CFLAGS) -c usb_validator.c

main_compile_command.o: main.c
	cc $(CFLAGS) -c main.c

compile_commands: main_compile_command.o
	make --always-make --dry-run

clean:
	rm -f *.o
