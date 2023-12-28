CXX := g++
CXXFLAGS:=-Wall -O3

SRCS = shader vertex_bundle sobject engine gobject component
OBJECTS = $(patsubst %, %.o, $(SRCS))
AR = ar crv

libflsengine.a: $(OBJECTS)
	$(AR) $@ $(OBJECTS)

shader.o: shader.cpp shader.h vertex_bundle.h texture.h sobject.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

vertex_bundle.o: vertex_bundle.cpp vertex_bundle.h texture.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

sobject.o: sobject.cpp sobject.h vertex_bundle.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

engine.o: engine.cpp engine.h typedef.h gobject.h component.h shader.h 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

gobject.o: gobject.cpp gobject.h engine.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

component.o: component.cpp component.h engine.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm *.o