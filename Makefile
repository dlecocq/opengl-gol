CC = g++
CCOPT = -O3 -Wall -g

FORMAT = TIFF

DEFINES = -DUSE_$(FORMAT)

LIBOPT = -framework GLUT -framework OpenGL -lGLEW

DEPENDENCIES = grapher.o gol.o shader_primitive.o

EXECUTABLE = glot

all : $(EXECUTABLE)

$(EXECUTABLE) : driver.o $(DEPENDENCIES)
	$(CC) $(CCOPT) -o $(EXECUTABLE) driver.o $(DEPENDENCIES) $(LIBOPT) -lpng -ltiff
	
driver.o : driver.cpp Makefile
	$(CC) $(CCOPT) -c driver.cpp

%.o : %.cpp %.h
	$(CC) $(CCOPT) $(DEFINES) -c $<

clean :
	rm -rdf $(DEPENDENCIES) $(EXECUTABLE) driver.o *~
	
tarball : $(SOURCE)
	cd ..
	mv src openglot
	tar -cvzf openglot.tar.gz openglot
	mv openglot src
	cd src
	