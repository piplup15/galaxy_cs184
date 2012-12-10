CC = g++ -O2
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DOSX
INCFLAGS = -I./glm-0.9.2.7 -I/usr/X11/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL \
		-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
		-lGL -lGLU -lm -lstdc++
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES 
INCFLAGS = -I./glm-0.9.2.7 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lfreeimage
endif

RM = /bin/rm -f 
all: galaxy
galaxy: main.o shaders.o Transform.o readfile.o display.o variables.h readfile.h shaders.h Transform.h grader.o UCB/grader.h ModelObj.o ModelObj.h Texture.o Texture.h
	$(CC) $(CFLAGS) -o galaxy shaders.o main.o Transform.o readfile.o display.o grader.o ModelObj.o Texture.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp shaders.h Transform.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
shaders.o: shaders.cpp shaders.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c shaders.cpp
readfile.o: readfile.cpp readfile.h variables.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
display.o: display.cpp variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c display.cpp
Transform.o: Transform.cpp Transform.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp  
grader.o: UCB/grader.cpp UCB/grader.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c UCB/grader.cpp
ModelObj.o: ModelObj.cpp ModelObj.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c ModelObj.cpp
Texture.o: Texture.cpp Texture.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Texture.cpp
clean: 
	$(RM) *.o galaxy *.png


 
