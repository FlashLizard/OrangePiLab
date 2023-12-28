
CROSS_COMPILE:=/work/OrangePi4/gcc-aarch64/bin/aarch64-none-linux-gnu-

CC:=gcc
CXX:=g++

CFLAGS:=-Wall -O3
LDFLAGS:=-Wall

INCLUDE := -I../common/external/include
LIB := -L../common/external/lib -ljpeg -lfreetype -lpng -lasound -lz -lc -lm

EXESRCS := ../common/graphic.c ../common/touch.c ../common/image.c ../common/task.c $(EXESRCS)


EXEOBJS := $(patsubst %.c, %.o, $(EXESRCS))
EXEOBJS := $(patsubst %.cpp, %.o, $(EXEOBJS))

$(EXENAME): $(EXEOBJS) $(SUBTASK)
	$(CXX) $(LDFLAGS) -o $(EXENAME) $(EXEOBJS) $(LIB) $(MYLIBS)
	mv $(EXENAME) ../out/

clean:
	rm -f $(EXENAME) $(EXEOBJS)

%.o: %.c ../common/common.h
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.cpp ../common/common.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
