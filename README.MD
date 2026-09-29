# MPU6050 Cube Visualizer
Can be used to check if MPU6050 is working properly. The Arduino reads the gyro and sends the rotation over serial, and a Python script turns a 3D cube on screen to match.

## Demo
[<video src="demo.mp4?raw=true" controls width="100%">
</video>](https://github.com/user-attachments/assets/40ebe7ae-746b-4d23-9eae-510af82b6e37)

## Tools Needed
1. Arduino Uno (for this specific .ino file but can be adjusted according to any board)
2. MPU6050 breakout board
3. Adafruit MPU6050 library
4. vPython (for 3D visualizer) and pyserial

## How to run it
1. Upload the .ino sketch to your Arduino
2. Leave the sensor still for a few seconds while it calibrates
3. Run python app.py. If your board isn't on /dev/ttyACM0, change the port in the file
4. Move the sensor around and watch the cube

## Things to know
1. If the cube rotates the wrong way on an axis, flip the matching SIGN_X, SIGN_Y or SIGN_Z at the top of app.py
2. It only uses the gyro, so the cube will drift slowly over time. Reset the Arduino to zero it
3. If nothing moves, check your I2C wiring (SCL and SDA) and the serial port
