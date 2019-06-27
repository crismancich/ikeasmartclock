# Ikea Stolpa Smart Clock Hack
Hacking the IKEA Stolpa Clock into a SmartClock

## How To
### What you need

Ikea Stolpa Wall Clock 24,99 €
https://www.ikea.com/de/de/p/stolpa-wanduhr-80400382/

1m WS2812B Adressable LED Strip with 60 LEDs per meter 15,99 €
https://www.amazon.de/gp/product/B01CDTEE5W

ESP8266 Nodemcu WIFI Microcontroller 5,99 €
https://www.amazon.de/AZDelivery-NodeMCU-ESP8266-ESP-12E-Development/dp/B06Y1LZLLY

## Figures
The part where the LED Strip is attached, has a circumference of 80cm.

### Why use the LED Strip with 60 LEDs / Meter?
Of course you can use Strips with 100 or 144 LEDs / Meter. But 60 LEDs per Meter are quite ideal. Here is why.

This Strip has 60 LEDs / 100cm so 0,6 LEDs / 1cm.
For 80cm this is 80 * 0,6 = 48 LEDs for 12 hours.
One hour is lighted with 48 / 12 = 4 LED.
That's perfect, so one LED equals 15 minutes.
