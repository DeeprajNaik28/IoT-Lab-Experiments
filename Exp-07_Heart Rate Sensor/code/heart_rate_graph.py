import serial
import time
import matplotlib.pyplot as plt
from collections import deque

# ==========================================
# SETTINGS
# ==========================================

SERIAL_PORT = "COM4"      # CHANGE THIS
BAUD_RATE = 9600

MAX_POINTS = 60           # Show last 60 readings

HIGH_BPM = 100
LOW_BPM = 40

# ==========================================
# CONNECT TO ARDUINO
# ==========================================

try:
    arduino = serial.Serial(
        SERIAL_PORT,
        BAUD_RATE,
        timeout=1
    )

    time.sleep(2)

    print("Connected to Arduino!")
    print("Starting live heart-rate graph...")

except serial.SerialException:
    print("Could not connect to Arduino.")
    print("Check the COM port.")
    print("Also make sure Arduino Serial Monitor is CLOSED.")
    exit()

# ==========================================
# DATA STORAGE
# ==========================================

bpm_data = deque(maxlen=MAX_POINTS)
time_data = deque(maxlen=MAX_POINTS)

start_time = time.time()

# ==========================================
# CREATE GRAPH
# ==========================================

plt.ion()

fig, ax = plt.subplots()

line, = ax.plot([], [], marker="o")

ax.set_title("Real-Time Heart Rate Monitor")
ax.set_xlabel("Time (seconds)")
ax.set_ylabel("Heart Rate (BPM)")

ax.set_ylim(30, 180)
ax.grid(True)

# ==========================================
# LIVE DATA LOOP
# ==========================================

try:

    while True:

        if arduino.in_waiting > 0:

            data = arduino.readline().decode(
                "utf-8",
                errors="ignore"
            ).strip()

            if data:

                try:

                    bpm = int(data)

                    # Ignore impossible values
                    if LOW_BPM <= bpm <= 180:

                        current_time = time.time() - start_time

                        bpm_data.append(bpm)
                        time_data.append(current_time)

                        # Print BPM
                        if bpm > HIGH_BPM:

                            status = "HIGH"

                        else:

                            status = "NORMAL"

                        print(
                            f"BPM: {bpm} | Status: {status}"
                        )

                        # ==================================
                        # UPDATE GRAPH
                        # ==================================

                        line.set_xdata(time_data)
                        line.set_ydata(bpm_data)

                        # Automatically adjust X axis
                        if len(time_data) > 1:

                            ax.set_xlim(
                                time_data[0],
                                time_data[-1] + 1
                            )

                        fig.canvas.draw()
                        fig.canvas.flush_events()

                except ValueError:
                    pass

        plt.pause(0.01)

except KeyboardInterrupt:

    print("\nMonitoring stopped.")

finally:

    arduino.close()
    plt.close()

    print("Arduino connection closed.")