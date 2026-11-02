import csv
import matplotlib.pyplot as plt


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
    

    # reading the csv file

    time = []
    voltage = []
    current = []
    distance = []

    with open("telemetry.csv", "r", newline="") as input_file: 
        reader = csv.DictReader(input_file)

        for row in reader: 
            time_s = float(row["Time (s)"])
            vBat_v = float(row["Battery Voltage (V)"])
            current_mA = float(row["Battery Current (mA)"])
            distance_mm = float(row["Distance in Front (mm)"])

            time.append(time_s)
            voltage.append(vBat_v)
            current.append(current_mA)
            distance.append(distance_mm)

    # plotting
    figure, axes = plt.subplots(3, 1, sharex=True)

    axes[0].plot(time, voltage)
    axes[0].set_title("Battery Voltage vs Time")
    axes[0].set_ylabel("Battery Voltage (V)")

    axes[1].plot(time, current)
    axes[1].set_title("Battery Current vs Time")
    axes[1].set_ylabel("Battery Current (mA)")

    axes[2].plot(time, distance)
    axes[2].set_title("Distance in Front vs Time")
    axes[2].set_xlabel("Time (s)")
    axes[2].set_ylabel("Distance in Front (mm)")    

    figure.tight_layout()
    plt.show()

    # simple stats
    print("Lowest Voltage:", min(voltage), "V")
    print("Highest Current:", max(current), "mA")
    print("Closest object in front:", min(distance), "mm")



main()
