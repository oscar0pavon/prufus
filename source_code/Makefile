CFLAGS := -I/usr/include -fdiagnostics-color=always -g
CFLAGS += $(shell pkg-config --cflags pixman-1)

WAYLAND_LIBS := -lEGL -lwayland-client -lwayland-egl
WAYLAND_LIBS += -lxkbcommon -lpway -lpfonts

CLINKS := -lX11 -lGL -lpthread -lm $(WAYLAND_LIBS)
CLINKS += $(shell pkg-config --libs pixman-1)

.PHONY: prufus

OBJS := window.o draw.o user_interface.o button.o device.o lodepng.o
OBJS += select_window.o input.o cpu_image.o

prufus: main.c $(OBJS)
	cc $(CFLAGS) main.c $(OBJS) -o ../prufus $(CLINKS) 

window.o: user_interface/window.c
	cc $(CFLAGS) -c $<

input.o: user_interface/input.c
	cc $(CFLAGS) -c $<

select_window.o: user_interface/select_window.c
	cc $(CFLAGS) -c $<

lodepng.o: user_interface/lodepng.c
	cc $(CFLAGS) -c $<

user_interface.o: user_interface/user_interface.c
	cc $(CFLAGS) -c $<

draw.o: user_interface/draw.c
	cc $(CFLAGS) -c $<

cpu_image.o: user_interface/cpu_image.c
	cc $(CFLAGS) -c $<

button.o: user_interface/button.c
	cc $(CFLAGS) -c $<

device.o: device.c
	cc $(CFLAGS) -c $<

main_compile_command.o: main.c
	cc $(CFLAGS) -c main.c

compile_commands: main_compile_command.o
	make --always-make --dry-run

clean:
	rm -f *.o
