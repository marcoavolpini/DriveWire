import csv


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


def main(): 

    telemetryData = [
    "0.0,7.8,250.0,600", 
    "0.1,7.7,260.0,580", 
    "0.2,7.7,280.2,540", 
    ]

    

    # opening a csv file
    with open("telemetry.csv", "w", newline="") as output_file: 
        writer = csv.writer(output_file)
        writer.writerow(["Time (s)","Battery Voltage (V)", "Battery Current (mA)", "Distance in Front (mm)"])
        
        for line in telemetryData: 
            time_s, vBat_v, current_mA, distance_mm = parse_telemetry(line)
            writer.writerow([time_s, vBat_v, current_mA, distance_mm])



main()
