# WeatherPaper_V2
> A minimalist, ultra-low power E-paper smart display that runs for 500+ days on a single charge. 

WeatherPaper is a health and comfort-focused smart weather station designed around a 4.2-inch e-paper display and a custom PCB. By heavily optimizing both the hardware design and the ESP32 firmware, this device achieves a deep-sleep current draw of just ~44µA, allowing its 1500mAh Li-Po battery to last over a year and a half.

## Repository Structure
* `/Hardware` - Contains Autodesk Fusion STL files and custom PCB Gerber manufacturing files.
* `/Firmware` - Contains the Arduino IDE production code (`WeatherPaper_Firmware.ino` and `bitmap.h`).

## Bill Of Material (BOM)
Microcontroller: Seeed Studio XIAO ESP32-C3

Display: WeAct 4.2" Black-White E-paper Module

Battery: 504050 3.7V 1500mAh LiPo Battery

Components: 4x 0603 1MΩ Resistors, 1x 0805 470nF Capacitor, 1x MSK12C02 Slide Switch
