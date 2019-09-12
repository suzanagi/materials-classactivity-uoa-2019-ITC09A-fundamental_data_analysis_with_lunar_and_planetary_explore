# Makefile for plate_renderer
CC = gcc

##Please choose your mission
#Hayabusa1
CFLAGS = -Wall -O2 -DHYB1
#Hayabusa2
#CFLAGS = -Wall -O2 -DHYB2

LDFLAGS =
INCLUDES = -I $(HOME)/Documents/Public/Course/LunarAndPlanetaryExplorations/cspice/include/ -I /opt/local/include/
LIBS = -lm -lcfitsio -L /opt/local/lib/\
        $(HOME)/Documents/Public/Course/LunarAndPlanetaryExplorations/cspice/lib/cspice.a
TARGET = plate_renderer
OBJS = plate_renderer.o plate_renderer_func.o get_cam_info_c.o get_plbore.o hapke.o ONC_getlos_c.o \
        

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	-rm -f $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

plate_renderer_func.o: plate_renderer.h
plate_renderer.o: plate_renderer.h
