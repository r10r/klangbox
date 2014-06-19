PA_MODULE_DIR := /usr/lib/pulse-5.0/modules
PA_SRC_DIR := /usr/local/src/pulseaudio

CFLAGS := -Wall -Werror -I $(PA_SRC_DIR)/src

MODULES := module-combine-unique \
	module-one-input \
	module-one-sink \
	module-couple

default: $(MODULES)

module-combine-unique: module-combine-unique.c util.c
	gcc $(CFLAGS) -g -shared -o $@.so $?

module-one-input: module-one-input.c
	gcc $(CFLAGS) -g -shared -o $@.so $?

module-one-sink: module-one-sink.c util.c
	gcc $(CFLAGS) -g -shared -o $@.so $?
	
module-couple: module-couple.c util.c
	gcc $(CFLAGS) -g -shared -o $@.so $?

pulsedevicelist: pulsedevicelist.c
	gcc $(CFLAGS) -o $@ $? -lpulse

install: $(MODULES)
	sudo cp $(patsubst %,%.so,$?) $(PA_MODULE_DIR)

deploy: install
	sudo -u pulse pacmd load-module module-combine-unique && \
	sudo -u pulse pacmd load-module module-one-input && \
	sudo -u pulse pacmd load-module module-one-sink && \
	sudo -u pulse pacmd load-module module-couple

	