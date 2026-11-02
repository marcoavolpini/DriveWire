import csv
import matplotlib.pyplot as plt
import serial


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

def analyze_csv(filename):
    # reading the csv file

    time = []
    voltage = []
    current = []
    distance = []

    with open(filename, "r", newline="") as input_file: 
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

    # simple stats
    print("Lowest Voltage:", min(voltage), "V")
    print("Highest Current:", max(current), "mA")
    print("Closest object in front:", min(distance), "mm")


    figure.tight_layout()
    plt.show()


def simulate_telemetry(sample_index): 
    time_s = sample_index * 0.1
    vBat_v = 7.8 - 0.001*sample_index
    current_mA = 250 + 0.5*sample_index
    distance_mm = 600 - 2*sample_index

    return f"{time_s},{vBat_v},{current_mA},{distance_mm}"
    

def main(): 

    # prepping live data stream
    live_time = []
    live_voltage = []
    live_current = []
    live_distance = []

    live_figure, live_axes = plt.subplots(3, 1, sharex = True)
    voltage_trace, = live_axes[0].plot([], []) # initializes plot and saves line object to voltage_trace
    current_trace, = live_axes[1].plot([], []) # comma because axes.plot returns a list
    distance_trace, = live_axes[2].plot([], [])


    #getting data from serial
    with serial.Serial("COM5", 115200, timeout=1) as port:
        # opening a csv file
        with open("telemetry.csv", "w", newline="") as output_file: 
            writer = csv.writer(output_file)
            writer.writerow(["Time (s)","Battery Voltage (V)", "Battery Current (mA)", "Distance in Front (mm)"])
        
            for index in range(100):
                raw_bytes = port.readline()
                line = raw_bytes.decode("utf-8", errors="replace").strip()

                try:
                    time_s, vBat_v, current_mA, distance_mm = parse_telemetry(line)
                except (ValueError, IndexError):
                    continue
                
                writer.writerow([time_s, vBat_v, current_mA, distance_mm])

                live_time.append(time_s)
                live_voltage.append(vBat_v)
                live_current.append(current_mA)
                live_distance.append(distance_mm)

                voltage_trace.set_data(live_time, live_voltage)
                current_trace.set_data(live_time, live_current)
                distance_trace.set_data(live_time, live_distance)

                for ax in live_axes:
                    ax.relim()
                    ax.autoscale_view()

                plt.pause(0.1)
    
    analyze_csv("telemetry.csv")



main()
