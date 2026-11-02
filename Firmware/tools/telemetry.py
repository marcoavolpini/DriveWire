telemetryData = "0.0,7.8,250.0,600"

def parse_telemetry(line): 
    # parsing commas
    dataPoints = line.split(",")

    # turning the text into numbers
    time_s = float(dataPoints[0])
    vBat_v = float(dataPoints[1])
    current_mA = float(dataPoints[2])
    distance_mm = float(dataPoints[3])

    # return the values
    return time_s, vBat_v, current_mA, distance_mm

result = parse_telemetry(telemetryData)
print(result)