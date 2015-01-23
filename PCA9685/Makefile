all: PCA9685.o PCA9685tester

HDRS = PCA9685.h PCA9685_Addresses.h
CXXFLAGS = -Wall -lwiringPi

PCA9685.o PCA9685tester.o: $(HDRS)

PCA9685tester: PCA9685.o PCA9685tester.o
	$(CXX) $^ -lwiringPi -o $@