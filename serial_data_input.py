import serial
import csv
from datetime import datetime
import time
import os

# ---------------------------
# USER SETTINGS
# ---------------------------
COM_PORT = 'COM3'              # Change to your actual port
BAUD_RATE = 115200             # Match with Serial.begin() in Arduino
FILE_NAME = 'PI-Controller_6.csv'  # <<< Manually specify file name here
TIME_LIMIT_SEC = None          # Set to None for continuous logging
# ---------------------------
# FILE NAME SETUP
# ---------------------------
file_name = FILE_NAME  # Use the manually provided file name

# ---------------------------
# SERIAL CONNECTION
# ---------------------------
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    print(f"[OK] Connected to {COM_PORT} at {BAUD_RATE} baud.")
except Exception as e:
    print(f"[ERROR] Could not connect to serial port: {e}")
    exit()

# ---------------------------
# CSV FILE SETUP
# ---------------------------
with open(file_name, mode='w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Time(ms)', 'Angle'])  # CSV Header

    print(f"[INFO] Logging started. Saving to: {file_name}")
    print("Press Ctrl+C to stop.\n")

    start_time = time.time()

    try:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f">> {line}")  # Show on screen
                parts = line.split(",")
                if len(parts) >= 2:
                    writer.writerow(parts[:2])  # Save Time(ms), Angle

            if TIME_LIMIT_SEC and (time.time() - start_time) > TIME_LIMIT_SEC:
                print("[INFO] Time limit reached. Logging stopped.")
                break

    except KeyboardInterrupt:
        print("\n[STOPPED] Logging stopped by user.")

    except Exception as e:
        print(f"[ERROR] {e}")

    finally:
        ser.close()
        print("[OK] Serial connection closed.")
