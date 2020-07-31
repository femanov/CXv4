CC              := g++
CFLAGS          := -I/usr/include/opencv -L/usr/lib64/
OBJECTS         := 
LIBRARIES       := -lopencv_core -lopencv_highgui -lpthread
CX_EXPORTS_DIR	:= /net/x10sae/home/user/work/4cx/exports
CX_LIBS		:= $(foreach L, cda cx_async useful mt_cxscheduler misc, $(CX_EXPORTS_DIR)/lib/lib$L.a)
CFLAGS		+= -I$(CX_EXPORTS_DIR)/include

.PHONY: all clean

all: test

test: 
	$(CC) $(CFLAGS) -o tvcapture tvcapture.cpp cda_adapter.c -ldl $(CX_LIBS) $(LIBRARIES)
        
clean:
	rm -f *.o
