uname_p := $(shell uname -p) # run uname to find architecture
$(info uname_p=$(uname_p))

ifeq ($(strip $(uname_p)),arm)
$(info makefile: arm64 detected, changing defaults)
CPP = /usr/bin/clang
CPPFLAGS = 
LDFLAGS = 
else
$(info makefile: no arm64 detected, preserving defaults)
CPP = /usr/bin/gcc
CPPFLAGS = -lm
LDFLAGS = -lm
# leave defaults
endif

all: recall shoot

recall: recall.o
	$(CPP) $^ -o $@ $(LDFLAGS)

shoot: shoot.o
	$(CPP) $^ -o $@ $(LDFLAGS)

recall.o: recall.c
	$(CPP) -c $(CPPFLAGS) $^

shoot.o: shoot.c
	$(CPP) -c $(CPPFLAGS) $^

clean:
	$(RM) *.o