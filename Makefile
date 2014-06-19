#LDFLAGS := -lpulse
# Build is for user account
CFLAGS := -Wall -Werror -I/usr/local/src/pulseaudio/src

default: module-hello-world

module-hello-world: module-hello-world.c
	gcc $(CFLAGS) -g -shared -o $@.so $?

pulsedevicelist: pulsedevicelist.c
	gcc $(CFLAGS) -o $@ $? -lpulse

install: module-hello-world
	sudo cp module-hello-world.so /usr/lib/pulse-5.0/modules
