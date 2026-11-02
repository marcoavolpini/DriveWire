telemetryData = [
    "0.0,7.8,250.0,600", 
    "0.1,7.7,260.0,580", 
    "0.2,7.7,280.2,540", 
]

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

for line in telemetryData: 
    time_s, vBat_v, current_mA, distance_mm = parse_telemetry(line)

    print(vBat_v)

