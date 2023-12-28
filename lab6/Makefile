
EXENAME := lab6
EXESRCS := main.cpp fb_tools.cpp test.cpp

SUBTASK := main fb_tools test libflsengine

include ../common/rules.mk

MYLIBS := fls_engine/libflsengine.a

libflsengine:
	cd fls_engine && make

main: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@.o $<
	touch $@

fb_tools: fb_tools.cpp fb_tools.h fls_engine/shader.h fls_engine/engine.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@.o $<
	touch $@

test: test.cpp fb_tools.h fls_engine/shader.h  fls_engine/engine.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@.o $<
	touch $@

clean:
	rm *.o