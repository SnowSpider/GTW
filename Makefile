CC= g++
INCLUDES= -I. -I./glut/GL/
CFLAGS+= -O2
LIBS= -L/usr/X11R6/lib -L/usr/lib -lXext -lX11 -lGL -lGLU -lglut
PROD= planet

HDRS= $(shell ls *.h)
SRCS= $(shell ls *.cpp)
OBJS= $(patsubst %.cpp, %.o, $(SRCS))

all: $(PROD)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

$(PROD): $(OBJS)
	$(CC) -o $(PROD) $^ $(LIBS)

clean:
	rm -f $(PROD)
	rm -f *.o

