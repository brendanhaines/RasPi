all: MPU6050dmp_tester

HDRS = helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h
CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall

I2Cdev.o MPU6050.o MPU6050dmp_tester.o: $(HDRS)

MPU6050dmp_tester: I2Cdev.o MPU6050.o MPU6050dmp_tester.o
	$(CXX) $^ -lm `pkg-config gtkmm-3.0 --cflags --libs` -lwiringPi -o $@

clean:
	rm I2Cdev.o MPU6050.o MPU6050dmp.o MPU6050dmp_tester.o MPU6050dmp_tester