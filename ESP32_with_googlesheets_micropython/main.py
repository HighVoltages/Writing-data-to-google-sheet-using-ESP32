import network
import urequests
import dht
from machine import Pin
from time import sleep
import ntptime
import time

# Define DHT sensor
dht_pin = Pin(4)
dht_sensor = dht.DHT22(dht_pin)

ntptime.host = 'pool.ntp.org'  # Use this or another known NTP server

# Button pins
button_send_pin = Pin(2, Pin.IN, Pin.PULL_UP)  # Button 1 for sending data
button_state_pin = Pin(16, Pin.IN, Pin.PULL_UP)  # Button 2 for state tracking

# WiFi credentials
ssid = 'Wokwi-GUEST'
password = ''

# Google Apps Script Web App URL
server_url = 'url'
# Connect to WiFi
def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    
    while not wlan.isconnected():
        print("Connecting to WiFi...")
        sleep(1)
    
    print("Connected to WiFi:", wlan.ifconfig())

# Get current NTP time
def get_ntp_time():
    try:
        ntptime.settime()  # Synchronize the system time with NTP
        tm = time.localtime(time.time())  # Get local time
        formatted_time = "{:02d}:{:02d}:{:02d}".format(tm[3], tm[4], tm[5])
        return formatted_time
    except Exception as e:
        print("Error getting NTP time:", e)
        return None

# Send data to the server
def send_data():
    # Read DHT sensor data
    try:
        dht_sensor.measure()  # Trigger the sensor reading
        humidity = dht_sensor.humidity()  # Get humidity
        temperature = dht_sensor.temperature()  # Get temperature
    except OSError as e:
        print("Failed to read from DHT sensor:", e)
        humidity = 0
        temperature = 0

    print(temperature,humidity)

    # Get button state
    button_state = button_state_pin.value() == 0  # True if pressed

    # Get current time
    timestamp = get_ntp_time()

    # Prepare JSON payload
    json_data = {
        "method": "replace",
        "temperature": temperature,
        "humidity": humidity,
        "timestamp": timestamp,
        "buttonState": str(button_state).lower()  # Convert bool to "true" or "false"
    }

    # Send HTTP POST request
    try:
        response = urequests.post(server_url, json=json_data)
        print("Response:", response.status_code, response.text)
        response.close()
    except Exception as e:
        print("Error sending data:", e)

# Main loop
def main_loop():
    while True:
        # Check if send button is pressed
        # if button_send_pin.value() == 0:  # Button is pressed
        print("Button pressed, sending data...")
        send_data()
        sleep(0.5)  # Debounce delay

        sleep(5)  # General loop delay

# Setup and run
if __name__ == "__main__":
    connect_wifi()
    main_loop()
