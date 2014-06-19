# cache evaluation of path
L := $(LOCAL_DIR)

CFLAGS += -shared -I/usr/local/src/pulseaudio/src

#TESTS += $(L)/test_

PROGRAMS += $(L)/module-couple.so
	# $(L)/module-combine-disjunct.so \
	# $(L)/module-couple.so \
	# $(L)/module-combine-disjunct.so \
	# $(L)/module-combine-disjunct.so

# overwrite CFLAGS per object
#$(L)/.o : CFLAGS += ...

# -- programs --

$(L)/module-couple.so_OBJS := module-couple.o
