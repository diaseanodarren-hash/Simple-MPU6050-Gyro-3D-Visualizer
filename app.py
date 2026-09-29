from vpython import box, rate, color
import serial
import struct
import math
import time

# Flip any of these to -1 if the cube spins the wrong way on that axis
SIGN_X = 1
SIGN_Y = 1
SIGN_Z = 1

ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
time.sleep(2)              # Uno resets when the port opens
ser.reset_input_buffer()

cube = box(color=color.white, opacity=0.6)

prev = None                # previous (x, y, z) angles in tenths of a degree


def read_packet():
    """Wait for the 0xAA 0x55 header, then return (x, y, z) in tenths of deg."""
    while True:
        b = ser.read(1)
        if not b:
            return None
        if b != b'\xaa':
            continue
        b = ser.read(1)
        if b != b'\x55':
            continue
        data = ser.read(6)
        if len(data) < 6:
            return None
        return struct.unpack('<hhh', data)


def angle_delta(new, old):
    """Shortest signed difference (in degrees), handles the -180/180 wrap."""
    d = (new - old + 1800) % 3600 - 1800
    return d / 10.0


def rotate_body(dx, dy, dz):
    """Rotate the cube about its OWN x, y, z axes (like a real gyro would)."""
    for delta, which in ((dx, 'x'), (dy, 'y'), (dz, 'z')):
        if delta == 0:
            continue
        x_axis = cube.axis.norm()
        y_axis = cube.up.norm()
        z_axis = x_axis.cross(y_axis)
        axis = {'x': x_axis, 'y': y_axis, 'z': z_axis}[which]
        cube.rotate(angle=math.radians(delta), axis=axis)


while True:
    packet = read_packet()
    if packet is None:
        continue

    rate(60)

    if prev is None:       # first packet: just remember it, no jump
        prev = packet
        continue

    dx = SIGN_X * angle_delta(packet[0], prev[0])
    dy = SIGN_Y * angle_delta(packet[1], prev[1])
    dz = SIGN_Z * angle_delta(packet[2], prev[2])
    prev = packet

    print(f"X:{packet[0] / 10:7.1f}  Y:{packet[1] / 10:7.1f}  Z:{packet[2] / 10:7.1f}")
    rotate_body(dx, dy, dz)
