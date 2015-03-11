all: PID.o I2Cdev.o MPU6050.o PCA9685.o DSM2.o PID.o

PCA_HDRS = PCA9685.h PCA9685_Addresses.h
MPU_HDRS = helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h
PID_HDRS = PID.h
DSM_HDRS = DSM2.h
HDRS = $(PID_HDRS) $(MPU_HDRS) $(PCA_HDRS)

CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall -lwiringPi

I2Cdev.o MPU6050.o PCA9685.o PID.o: $(HDRS)
PCA9685tester.o echoServerAdvanced.o main.o: $(HDRS)

v2Parser.o: v2Parser.h helper_3dmath.h
PID.o: $(PID_HDRS)

DSM2.o DSM2tester.o: $(DSM_HDRS)
DSM2tester: DSM2tester.o DSM2.o
	$(CXX) $^ -lwiringPi -o $@

v2main.o: v2Parser.h $(PCA_HDRS) $(MPU_HDRS) $(PID_HDRS)
v2main: v2main.o v2Parser.o PCA9685.o PID.o I2Cdev.o MPU6050.o DSM2.o
	$(CXX) $^ -lwiringPi -o $@

v2EchoServer: v2EchoServer.cpp
	$(CXX) $^ -o $@

main: main.o PID.o PCA9685.o I2Cdev.o MPU6050.o
	$(CXX) $^ -lwiringPi -o $@

PCA9685tester: PCA9685.o PCA9685tester.o
	$(CXX) $^ -lwiringPi -o $@

echoServerAdvanced: echoServerAdvanced.o PCA9685.o
	$(CXX) $^ -lwiringPi -o $@

SatelliteReceiver.o SatelliteReceiverTest.o: SatelliteReceiver.h
SatelliteReceiverTest: SatelliteReceiver.o SatelliteReceiverTest.o
	$(CXX) $^ -lwiringPi -o $@

clean:
	rm I2Cdev.o MPU6050.o MPU6050dmp.o MPU6050dmp_tester.o MPU6050dmp_tester PID.o main.o main PCA9685tester