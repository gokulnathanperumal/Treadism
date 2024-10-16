# <img src="https://github.com/user-attachments/assets/79b2ac56-1bd7-45da-be18-08a86d90e584" width="5%" align="left">Treadism

A **Internet of Things (IoT)** project that integrates a treadmill with an **Arduino Microcontroller** and an **Android Application**. It enables users to configure custom interval training programs and perform hands-free operations with voice control, utilizing **Bluetooth LE** for communication. This project provides a seamless and customizable workout experience, whether for fitness enthusiasts or individuals undergoing rehabilitation.

<img src="https://github.com/user-attachments/assets/e24b8770-d333-4715-921c-998fc3fa981b" width="22.5%">
<img src="https://github.com/user-attachments/assets/530e36cf-cd7f-4914-84bd-20d30d727692" width="22.5%">
<img src="https://github.com/user-attachments/assets/519d48e3-2830-4db1-9545-5ce9e268de3b" width="22.5%">
<img src="https://github.com/user-attachments/assets/e57ed287-0293-46f2-9895-360574c22246" width="22.5%">
<img src="https://github.com/user-attachments/assets/34e2d51e-f9cc-4425-b633-b65d64b6bbe3" width="22.5%">
<img src="https://github.com/user-attachments/assets/0fd9dfad-b787-4f71-aae8-22822777983a" width="22.5%">
<img src="https://github.com/user-attachments/assets/34b4ee50-d55f-43b3-9fd7-7101b1fb6713" width="22.5%">
<img src="https://github.com/user-attachments/assets/269395de-8eb7-4b3b-9ec8-8107e1b1c9d6" width="22.5%">

## Features

- **Custom Interval Training Programs**: Create personalized workout programs tailored to your fitness goals.
- **Voice Control**: Hands-free operation of treadmill functions like start, stop, and speed control.
- **Bluetooth LE Communication**: Seamless wireless control between the Arduino microcontroller and Android app.
- **User-Friendly Interface**: The mobile app is designed using MIT App Inventor for easy setup and use.
- **Arduino Integration**: Full control of the treadmill using an Arduino microcontroller with HM-10 Bluetooth module.

## Technology Involved

- **Embedded System**: Arduino Microcontroller with Bluetooth Module HM-10.
- **Communication Protocols**: I2C (between the treadmill and Arduino), UART (between Arduino and HM-10), Bluetooth LE.
- **Frontend**: Android Mobile App development using MIT App Inventor.
- **Voice Control**: Hands-free treadmill operation using voice commands.

## Requirements

- **Hardware**:
  - Treadmill with accessible I2C connections.
  - Arduino (e.g., Arduino Uno or Nano).
  - HM-10 Bluetooth module.
  - Wires for communication setup (SCL, SDA, GND).
  
- **Software**:
  - [MIT App Inventor](https://appinventor.mit.edu/) for Android app development.
  - Arduino IDE for coding the Arduino Microcontroller.
  
## Setup Guide

### 1. Hardware Setup

- Connect the treadmill and Arduino using I2C for communication:
  - **Treadmill SCL** to **Arduino SCL**.
  - **Treadmill SDA** to **Arduino SDA**.
  - **Treadmill GND** to **Arduino GND**.
  
- For UART communication between the Arduino and HM-10 Bluetooth module:
  - **HM-10 TX** to **Arduino RX**.
  - **HM-10 RX** to **Arduino TX**.
  - **HM-10 VCC** to **Arduino 3.3V**.
  - **HM-10 GND** to **Arduino GND**.

### 2. Arduino Firmware

- Upload the Arduino code to manage the treadmill’s speed, start/stop functions, and Bluetooth communication. 
- Make sure to disconnect RX and TX pins when uploading the code to the Arduino.

### 3. Android App

- Use **MIT App Inventor** to develop and deploy the Treadism app on your Android device.
- Ensure Bluetooth and location are turned on in your phone settings before connecting to the treadmill.

### 4. Pairing and Control

- Open the Treadism app, and pair it with the treadmill via the HM-10 Bluetooth module.
- Choose from pre-configured interval training programs or create your own.
- Use voice commands for hands-free control during workouts.

## How to Use

1. **Turn on the treadmill and the Arduino-based controller.**
2. **Ensure the treadmill display shows "GO."** If not, press the stop button on the treadmill.
3. **Enable Bluetooth and location services on your phone.**
4. **Open the Treadism app.**
5. **Pair with the “TM Run 100” device.**
6. **Select your desired workout program and start.**

## Customization

You can modify the interval training programs or the voice commands by adjusting the code in the Arduino sketch or updating the MIT App Inventor project to fit your preferences.

## Project Motivation

This project was developed to enhance treadmill workout experiences, allowing more customization and accessibility. It was particularly beneficial during a personal recovery period following forearm fracture surgery, where voice control helped with hands-free treadmill operation.
