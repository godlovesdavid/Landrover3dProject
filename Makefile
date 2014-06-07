BASE = SdlApp

all: $(BASE)

OS := $(shell uname -s)

ifeq ($(OS), Linux) # Science Center Linux Boxes
  CPPFLAGS = -I/home/l/i/lib175/usr/glew/include
  LDFLAGS += -L/home/l/i/lib175/usr/glew/lib -L/usr/X11R6/lib
  LIBS += -lGL -lGLU -lSDL2 -lSDL2_image
endif

ifeq ($(OS), Darwin) # Assume OS X
  CPPFLAGS += -D__MAC__ -I/usr/local/Cellar/glew/1.10.0/include/ --stdlib=libstdc++ -Wno-deprecated
  LDFLAGS += -framework SDL2 -framework SDL2_image -framework OpenGL -L/usr/local/Cellar/glew/1.10.0/lib/
endif

ifdef OPT 
  #turn on optimization
  CXXFLAGS += -O2
else 
  #turn on debugging
  CXXFLAGS += -g
endif

CXX = g++ 

OBJ = $(BASE).o ppm.o glsupport.o

$(BASE): $(OBJ)
	$(LINK.cpp) -o $@ $^ $(LIBS) -lGLEW 

clean:
	rm -f $(OBJ) $(BASE)
