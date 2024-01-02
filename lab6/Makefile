
EXENAME := lab6
EXESRCS := main.cpp fb_tools.cpp test.cpp

SUBTASK := libflsengine
MYLIBS := fls_engine/libflsengine.a

include ../common/rules.mk

vpath %.h fls_engine/include

libflsengine:
	cd fls_engine && make

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

fb_tools.o: fb_tools.cpp fb_tools.h engine.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

test.o: test.cpp fb_tools.h engine.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm *.o; cd fls_engine && make clean