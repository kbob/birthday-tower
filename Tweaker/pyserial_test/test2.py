import serial
import time

while True:
    try:
        # port = serial.serial_for_url('hwgrep://16C0')
        port = serial.serial_for_url('hwgrep://16C0&n=2')
    except serial.serialutil.SerialException:
        time.sleep(1)
        continue
    time.sleep(0.2)
    port.write(b'list\n')
    while True:
        line = port.readline()
        print(f'{line!r}')
        if line == b'-\n':
            break
    while True:
        try:
            port.cts
        except OSError as x:
            print(f'{x}')
            del port
            time.sleep(5) # let Teensyduino run
            break
        time.sleep(1)