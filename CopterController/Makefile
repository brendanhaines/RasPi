all: pid.o I2Cdev.o MPU6050.o PCA9685.o PCA9685tester pidTest
#pidTest MPU6050dmp_tester PCA9685tester

HDRS = pid.h helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h PCA9685.h PCA9685_Addresses.h
CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall -lwiringPi

pid.o pidTest.o I2Cdev.o MPU6050.o MPU6050dmp_tester.o PCA9685.o PCA9685tester.o: $(HDRS)

pidTest: pidTest.o pid.o PCA9685.o I2Cdev.o MPU6050.o
	$(CXX) $^ -lwiringPi -o $@

MPU6050dmp_tester: I2Cdev.o MPU6050.o MPU6050dmp_tester.o
	$(CXX) $^ -lm `pkg-config gtkmm-3.0 --cflags --libs` -lwiringPi -o $@

PCA9685tester: PCA9685.o PCA9685tester.o
	$(CXX) $^ -lwiringPi -o $@

clean:
	rm I2Cdev.o MPU6050.o MPU6050dmp.o MPU6050dmp_tester.o MPU6050dmp_tester pid.o pidTest.o pidTest PCA9685tester