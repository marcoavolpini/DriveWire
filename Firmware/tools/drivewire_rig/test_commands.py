import serial
from board import send_command, send_raw, read_lines

def test_vaild_command_gets_ok(): 
    with serial.Serial('COM4', 115200, timeout=1) as port: 
        send_command(port, 0, 0)
        lines = read_lines(port, 0.5)

        assert 'OK' in lines
        assert 'ERR' not in lines

        print(', '.join(lines))

