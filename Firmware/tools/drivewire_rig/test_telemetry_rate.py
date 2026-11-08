from board import send_command, read_lines, parse_telemetry
import serial


def test_telemetry_rate(): 
    with serial.Serial('COM4', 115200, timeout=1) as port: 
        lines = read_lines(port, 2)
        periods = []
        times = []
        for line in lines: 
            values = parse_telemetry(line)
            if values != None: 
                time_s, _, _, _ = values
            else: 
                continue
            if (len(times) != 0): 
                period = time_s - times[-1]
            else: 
                times.append(time_s) 
                continue
            times.append(time_s)
            periods.append(period)

            assert period > 0.09
            assert period < 0.11
        assert len(periods) > 15

        

