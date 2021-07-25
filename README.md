# TetherCar
Github Repo For Tether Car Project 

> # Requirements:
> In order to use the code I've written, you must include the necessary libraries in the Arduino IDE. Assuming you've already added the 
> board itself after following the instructions [here](https://learn.adafruit.com/adafruit-feather-m0-adalogger/setup), this is done by 
> clicking "Tools" OR "Sketch"->"Include Libraries" then "Manage Libraries". You'll need to atleast include the MPRLS library and the 
> MAX31855 library, so search for those. I think it will ask if you want to install their additional dependencies, so answer "yes". The 
> other three libraries that appear at the top of the "M0Logger" file should be included with the IDE by default.

> # Wiring
> It shouldn't matter if you're powering it off USB or from a lipo battery, just hook up the "3v" pin to power on the breadboard and a "gnd" pin to gnd on the breadboard. This will supply everything with 3.3v, which is what they operate off of anyway, though the total current draw must be less than 400mA, which might be a problem if a bunch more sensors are added later.
> <br>
> Here's a table for the rest of the pins:
> | Adalogger Pin    | Peripheral Pin |
> | ----------- | ----------- |
> | 12     | MAX31855 DO      |
> | 11   | MAX31855 CS       |
> | 10      | MAX31855 CLK       |
> | A4   | TMP output leg        |
> | A1      | ADXL377 X       |
> | A2   | ADXL377 Y        |
> | A3      | ADXL377 Z       |
> | 5   | Hall output leg        |
>
> Also don't forget to include the 10k pullup between the output and power pins across the hall effect sensor, and maybe include the capacitors and other fun stuff included with its datasheet.