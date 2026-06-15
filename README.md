# WeatherPaper_V2
> A minimalist, ultra-low power E-paper smart display that runs for 500+ days on a single charge. 

WeatherPaper is a health and comfort-focused smart weather station designed around a 4.2-inch e-paper display and a custom PCB. By heavily optimising both the hardware design and the ESP32 firmware, this device achieves a deep-sleep current draw of just ~44µA, allowing its 1500mAh Li-Po battery to last over a year and a half.

**Looking for the step-by-step build guide?** Check out the full project write-up on [Instructables](https://www.instructables.com/WeatherPaper-Squeezing-500-Days-of-Battery-Life-Ou/)!

## Features

## Firmware Optimisations

## Repository Structure
* `/Hardware`: Contains Autodesk Fusion STL files and custom PCB Gerber manufacturing files.
* `/Firmware`: Contains the Arduino IDE production code.

## Customising UI & Icons
### Swapping Data variables
### Changing UI graphics & icons

## Bill Of Material (BOM)
Microcontroller: 
1. Seeed Studio XIAO ESP32-C3 - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c2ubReAp)

Display: 
1. WeAct 4.2" Black-White E-paper Module - [Buy in AliExpress](https://www.aliexpress.com/item/1005008461198386.html)

Battery: 
1. 504050 3.7V 1500mAh LiPo Battery - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3bztVFF)

PCB Components: 
1. 4x 0603 1MΩ Resistors - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c36aE6Wl)
2. 1x 0805 470nF Capacitor - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c4o0gDF7)
3. 1x MSK12C02 Slide Switch - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3pcpaaV)
4. 8+2 Pins 2.54mm Pin Header Male - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3S1g8Zn)
5. 8+2 Pins 2.54mm Pin Header Female - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3S1g8Zn)

Tools:
1. Soldering Station (any) - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3TB0DUp) (<= this is the one I used)
2. Double-sided adhesive tape - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c44TEvrn)

## License
This project is licensed under the GNU General Public License v2.0. See the [LICENSE](https://github.com/ngai-jeremy/WeatherPaper_V2/blob/main/LICENSE) file for details.

## Acknowledgements
- Seeed Studio for designing the ultra-power-efficient XIAO ESP32-C3 platform.
- OpenWeatherMap API for providing public real-time weather and air quality data.
