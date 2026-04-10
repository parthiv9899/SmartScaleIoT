import serial
import time
import firebase_admin
from firebase_admin import credentials, db

# --- Firebase Init ---
cred = credentials.Certificate("serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://load-cell-ea878-default-rtdb.firebaseio.com/'
})
ref = db.reference('weight_data')

# --- Serial Init ---
# Change 'COM3' to your port → Windows: COM3/COM4, Linux: /dev/ttyUSB0
ser = serial.Serial('COM6', 57600, timeout=1)
time.sleep(2)  # Wait for Arduino to reset

print("✅ Connected. Listening to weighing machine...\n")

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            continue

        print(f"RAW → {line}")

        # Parse lines like: "Weight (g): 172.3"
        if line.startswith("Weight (g):"):
            try:
                weight_str = line.replace("Weight (g):", "").strip()
                weight = float(weight_str)

                data = {
                    "weight_g": weight,
                    "timestamp": time.time(),
                    "datetime": time.strftime('%Y-%m-%d %H:%M:%S')
                }

                ref.push(data)
                print(f"✅ Pushed to Firebase: {data}\n")

            except ValueError:
                print(f"⚠️  Could not parse weight from: {line}\n")

        elif line.startswith("Tare"):
            print(f"ℹ️  Tare event: {line}\n")

        else:
            print(f"⏭️  Skipped (debug line): {line}\n")