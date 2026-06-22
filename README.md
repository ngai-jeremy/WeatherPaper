# WeatherPaper
> A minimalist, ultra-low power E-paper smart display that runs for 500+ days on a single charge.

![WeatherPaper Hero Image](https://content.instructables.com/FQ0/HTFH/MPX8P88O/FQ0HTFHMPX8P88O.jpg)

WeatherPaper is a health and comfort-focused smart weather station designed around a 4.2-inch e-paper display and a custom PCB. By heavily optimising both the hardware design and the ESP32 firmware, this device achieves a current draw per cycle of just ~0.0494mAh, allowing its 1500mAh Li-Po battery to last over a year and a half.

**Looking for the step-by-step build guide?** Check out the full project write-up on [Instructables](https://www.instructables.com/WeatherPaper-Squeezing-500-Days-of-Battery-Life-Ou/)!

Thanks to the XDA Developers team, this project has been featured in a write-up: [XDA Developers](https://www.xda-developers.com/this-esp32-e-paper-weather-tracker-runs-off-one-battery-charge-for-15-years/)

## Features
* **500 Days Battery Life:** WiFi fetching uptime cuts to a fraction of a second, driven by aggressive power management in software optimisation and firmware downclocking. 
* **Seamless WiFi Captive Setup:** By utilising `WiFiManager` captive portal, there's no need to hardcode sensitive WiFi credentials or API keys into the source code.
* **Frosted Resin Enclosure:** Internal components subtly become part of WeatherPaper's aesthetics. Not something to hide.
* **Flush Wall Mount:** Built-in screw mount designed directly into the back panel for clean, seamless wall hanging.

## Firmware Optimisations
* **CPU Downclocking:** Halved CPU frequency to just 80MHz, cutting power comsumption by 15-30%.
* **Static IP Configuration:** Skips default DHCP negotiation, shortening wake-up time by 1-3 seconds.
* **HTTP instead of HTTPS:** Bypass complex mathematical TLS handshake for fetching public data.
* **HTTP Keep-Alive:** Handles multiple HTTP requests in a single TCP connection batch, keeping total active wake time down to just **4.3 seconds** per refresh cycle.

## Repository Structure
* `/Hardware`: Contains Autodesk Fusion STL files and custom PCB Gerber manufacturing files.
* `/Firmware`: Contains the Arduino IDE production code.

## Customising UI & Icons
WeatherPaper is open-source and highly customisable if you want to customise the data shown.

### Swapping Data variables
Open `WeatherPaperV2_Firmware.ino` and look for the JSON data parsing section. For example: `windDisp = doc["current"]["wind_speed"];`
Change the parsed JSON key to match the data fields you want from the OpenWeatherMap One Call 3.0 API (e.g., changing `wind_speed` to `uvi` for UV Index).

### Changing UI graphics & icons
Navigate to `bitmaps.h` tab in Arduino IDE. This tab contains all the arrays for all the graphics layouts. You can design a monochrome icon in Photoshop or Lopaka, convert it using tools like image2cpp and overwrite existing array.

## Bill Of Material (BOM)
Microcontroller: 
1. Seeed Studio XIAO ESP32-C3 - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c2RZpD5P)

Display: 
1. WeAct 4.2" Black-White E-paper Module - [Buy in AliExpress](https://www.aliexpress.com/item/1005008461198386.html)

Battery: 
1. 504050 3.7V 1500mAh LiPo Battery - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3AlFwAz)

PCB Components: 
1. 4x 0603 1MΩ Resistors - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3NsuvBF)
2. 1x 0805 470nF Capacitor - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3pQLw5L)
3. 1x MSK12C02 Slide Switch - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c3mGFjHJ)
4. 8+2 Pins 2.54mm Pin Header Male - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c4aDgPf3)
5. 8+2 Pins 2.54mm Pin Header Female - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c4aDgPf3)

Tools:
1. Soldering Station (any) - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c4r5LdUN) (<= this is the one I used)
2. Double-sided adhesive tape - [Buy in AliExpress](https://s.click.aliexpress.com/e/_c44TEvrn)

## License
This project is licensed under the GNU General Public License v2.0. See the [LICENSE](https://github.com/ngai-jeremy/WeatherPaper_V2/blob/main/LICENSE) file for details.

## Acknowledgements
- Seeed Studio for designing the ultra-power-efficient XIAO ESP32-C3 platform.
- OpenWeatherMap API for providing public real-time weather and air quality data.
