import pandas as pd
import matplotlib.pyplot as plt

# File name yaha dalna hai
df = pd.read_csv('PI-Controller_6.csv')

plt.figure(figsize=(12, 6))

plt.plot(df['Time(ms)'], df['Angle'], label='MPU Data (Angle)', color='blue')

plt.axhline(y=0, color='black', linestyle='--', linewidth=1.2, label='Reference (0°)')

# Add labels and title
plt.xlabel('Time (milliseconds)')
plt.ylabel('Angle (degrees)')
plt.title('Time vs Angle (with 0° Reference)')
plt.legend()
plt.grid(True)

# Adjust layout
plt.tight_layout()
plt.show()
