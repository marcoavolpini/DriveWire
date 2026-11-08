import serial
import time

# file for communicating with board over serial

def send_command(port, left, right): 
    cmd = f'M,{left},{right}'

    send_raw(port, cmd)

def read_lines(port, seconds):
    # why are we doing seconds for this

    lines = []

    start_time = time.time()
    while time.time() < seconds + start_time: 
        line_bytes = port.readline()

        try: 
            line = line_bytes.decode()

        except UnicodeDecodeError:      # safety for junk bytes during boot up
            line = ''

        line = line.strip() #it was sent using println so good to add

        if line != '':
            lines.append(line)

    return lines

def send_raw(port, text): 
    line = f'{text}\n'
    line_bytes = line.encode()
    port.write(line_bytes)

def parse_telemetry(line): 
    try: 
        time_s, voltage_V, current_mA, distance_mm =  line.split(',')
        return (float(time_s), float(voltage_V), float(current_mA), float(distance_mm))
    except ValueError: 
        return None


if __name__ == '__main__': 
    with serial.Serial('COM4', 115200, timeout=1) as port: 
        send_command(port, 0, 0)
        print(', '.join(read_lines(port, 1)))

        send_command(port, 255, 255)
        print(', '.join(read_lines(port, 2)))