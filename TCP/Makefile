all: TCPserver.o TCPserverExample

HDRS = TCPserver.h
CXXFLAGS = -Wall

TCPserver.o TCPserverExample.o: $(HDRS)

TCPserverExample: TCPserver.o TCPserverExample.o
	$(CXX) $^ -o $@

clean:
	rm TCPserver.o TCPserverExample.o TCPserverExample
