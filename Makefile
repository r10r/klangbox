#LDFLAGS := -lpulse
# Build is for user account 
CFLAGS := -I/usr/local/src/pulseaudio/src

default: module-hello-world.so

module-hello-world.so:
	gcc $(CFLAGS) -g -shared -o module-hello-world.so module-hello-world.c
