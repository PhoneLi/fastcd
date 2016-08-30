#code by phoneli

.PHONY : all clean
TARGETS = fastcd
all : $(TARGETS)

LIB = -pthread -lncurses
CFLAGS = -Wall

CC = g++ $(CFLAGS)

#make r=0 | make r=1
r = 0
ifeq ($(r),0)
	# debug
	CFLAGS += -g
else
	# release
	#CFLAGS += -static -O3 -DNDEBUG
	CFLAGS += -O3 -DNDEBUG
endif

fastcd: 
	$(CC) $@.cpp -o $@.out $(LIB)

#Tool command
clean:
	-rm -rf *.out ./tmp/*
