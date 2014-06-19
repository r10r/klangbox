PA_MODULE_DIR := /usr/lib/pulse-5.0/modules
PA_SRC_DIR := /usr/local/src/pulseaudio

CFLAGS := -Wall -Werror -I $(PA_SRC_DIR)/src

MODULES := module-combine-unique.so \
	module-one-input.so \
	module-one-sink.so \
	module-couple.so

default: $(MODULES)

%.so: %.c util.c
	gcc $(CFLAGS) -g -shared -o $@ $?

pulsedevicelist: pulsedevicelist.c
	gcc $(CFLAGS) -o $@ $? -lpulse

clean:
	rm $(wildcard $(MODULES))

install: $(MODULES)
	sudo cp $(MODULES) $(PA_MODULE_DIR)

load-modules: install
	sudo -u pulse pacmd load-module module-combine-unique && \
	sudo -u pulse pacmd load-module module-one-input && \
	sudo -u pulse pacmd load-module module-one-sink && \
	sudo -u pulse pacmd load-module module-couple

	