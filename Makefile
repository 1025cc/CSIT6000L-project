UNAME := $(shell uname)
INCFLAGS  = -I vecmath/include
# INCFLAGS += -I /usr/include/GL

FRAMEWORKS = -framework Glut
FRAMEWORKS += -framework OpenGL

LINKFLAGS = -L. -lRK4 -lglut -lGL # -lGLU
CFLAGS    = -O2 -Wall -ansi -DSOLN
CC        = g++
SRCS      = $(wildcard *.cpp)
SRCS     += $(wildcard vecmath/src/*.cpp)
OBJS      = $(SRCS:.cpp=.o)
PROG      = a3

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(FRAMEWORKS) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
