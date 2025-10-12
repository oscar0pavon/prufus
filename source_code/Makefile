CFLAGS := -I/usr/include -fdiagnostics-color=always

CLINKS := -lX11 -lGL -lpthread -lm

.PHONY: prufus

OBJS := window.o opengl.o user_interface.o button.o

prufus: main.c $(OBJS)
	cc $(CFLAGS) main.c $(OBJS) -o ../prufus $(CLINKS) 
window.o: user_interface/window.c
	cc $(CFLAGS) -c $<

user_interface.o: user_interface/user_interface.c
	cc $(CFLAGS) -c $<

opengl.o: user_interface/opengl.c
	cc $(CFLAGS) -c $<

button.o: user_interface/button.c
	cc $(CFLAGS) -c $<

usb_validator.o: usb_validator.c
	cc $(CFLAGS) -c usb_validator.c

main_compile_command.o: main.c
	cc $(CFLAGS) -c main.c

compile_commands: main_compile_command.o
	make --always-make --dry-run

clean:
	rm -f *.o
