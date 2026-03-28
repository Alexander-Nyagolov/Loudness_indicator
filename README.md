# Loudness indicator
<table>
  <tr>
    <th>
      Using a standard electret microphone and a preamplifier, noise levels are measured, and the data is received and processed by the ESP32. The readings are compared against limits set by international institutions and the Regional Inspectorate of Environment and Water, and displayed as a color scheme (green, yellow, red) on an addressable LED strip. At the same time, the data is sent via the MQTT protocol to an IoT platform. The device is powered directly from a power outlet, with a built-in step-down converter. The code includes OTA updates to ensure accessibility after the device is manufactured and the enclosure is sealed.
    </th>
    <th>
    <img width="200" height="800" alt="Thingspeak" src="https://github.com/user-attachments/assets/58d024a4-051d-496f-beb5-7ead1bf4e355" />
    </th>
  </tr>
</table>

<img width="1500" height="400" alt="Devices" src="https://github.com/user-attachments/assets/249256e7-0e09-46a7-8940-30a1797995e2" />
