# Automated Lamp Timer Clock / Lights Controller

An Arduino based clock with an automated lamp scheduling system, featuring an LCD display and a menu driven interface for configuration.

## Purpose

This project was created to automate lamp control based on a daily schedule. It is ideal for situations where you want lights to turn on and off automatically at specific times, such as for plant grow lights, aquarium lighting, home automation, or simply as a programmable night light.

The device displays the current date and time when idle and allows users to configure the lamp schedule through an intuitive menu system controlled by buttons.

## Why These Components?

| Component | Purpose |
|-----------|---------|
| **Arduino (Uno/Nano)** | The microcontroller serves as the brain of the project. It is affordable, suitable for beginners, and has enough I/O pins for all peripherals. |
| **DS1302 RTC Module** | Provides accurate timekeeping even when the Arduino is powered off (battery backed). Essential for reliable scheduling since the Arduino internal clock resets on power loss. |
| **16x2 I2C LCD Display** | Displays the current time and menu interface. The I2C version was chosen to minimize wiring (only 2 data pins instead of 6+), freeing up GPIO pins for buttons. |
| **4 Push Buttons** | Provide navigation through the menu system (Up, Down, Left, Right) to configure settings without needing a computer connection. |
| **Relay or LED on Pin 13** | Controls the lamp. Pin 13 was chosen for easy testing with the built in LED. In production, this would connect to a relay module for switching AC lamps. |

## Features

- Real time clock display (date and time)
- Configurable lamp turn on time (hour and minutes)
- Configurable lamp turn off time (hour and minutes)
- Supports schedules that span midnight (e.g., 22:00 to 06:00)
- Automatic LCD backlight timeout (5 minutes) to save power
- Menu automatically exits after 20 seconds of inactivity
- Debounced button inputs for reliable navigation

## Hardware Requirements

- Arduino Uno, Nano, or compatible board
- DS1302 RTC module
- 16x2 LCD with I2C backpack (address 0x27)
- 4× Momentary push buttons
- Relay module (for controlling AC lamps) or use the onboard LED for testing
- Connecting wires
- Breadboard (optional, for prototyping)

## Wiring Diagram

### LCD Display (I2C)

| LCD Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5V          |
| GND     | GND         |
| SDA     | A4          |
| SCL     | A5          |

### DS1302 RTC Module

| RTC Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5V          |
| GND     | GND         |
| IO      | 4           |
| SCLK    | 5           |
| CE      | 2           |

### Push Buttons

All buttons are configured with internal pullup resistors. Connect one side of each button to the Arduino pin and the other side to GND.

| Button   | Arduino Pin | Function                    |
|----------|-------------|-----------------------------|
| Left     | 8           | Navigate left / Decrease value |
| Right    | 9           | Navigate right / Increase value |
| Up       | 10          | Go back / Exit menu         |
| Down     | 11          | Enter submenu / Confirm     |

### Lamp Output

| Output   | Arduino Pin |
|----------|-------------|
| Lamp/Relay | 13        |

> **Note:** For controlling AC lamps, use a relay module between pin 13 and the lamp. Never connect AC voltage directly to the Arduino.

## Menu Navigation

The menu system is navigated using the four buttons:

When the clock is displayed (idle state), pressing any button will enter the Settings menu.

**In the Settings menu:**
- Press **Down** to enter the submenu and browse through the available settings.
- Press **Up** to exit back to the clock display.

**Browsing settings:**
- Press **Left** or **Right** to move between different settings (Lamp Turn On Hour, Lamp Turn On Minutes, Lamp Turn Off Hour, Lamp Turn Off Minutes).
- Press **Down** to edit the selected setting.
- Press **Up** to go back to the main Settings menu.

**Editing a value:**
- Press **Left** to decrease the value.
- Press **Right** to increase the value.
- Press **Up** or **Down** to confirm and return to the previous screen.

## Installation

1. **Install Required Libraries**
   
   Open the Arduino IDE and install the following libraries via the Library Manager (`Sketch → Include Library → Manage Libraries`):
   
   - `LiquidCrystal_I2C` by Frank de Brabander
   - `Rtc by Makuna` (for RtcDS1302)

2. **Set the Initial Time**
   
   On the first upload (or after replacing the RTC battery), uncomment these lines in `setup()` to set the clock:
   
   ```cpp
   // Serial.println("Overrite the current clock time!");
   // Rtc.SetIsWriteProtected(false);
   // Rtc.SetDateTime(compiled);
   ```
   
   Upload the sketch, then **comment them out again** and upload once more to prevent the time from resetting on every power cycle.

3. **Upload the Sketch**
   
   Connect your Arduino and upload the code.

## Configuration

Default settings (can be changed via the menu):

| Setting          | Default Value |
|------------------|---------------|
| Lamp Turn On     | 09:00         |
| Lamp Turn Off    | 18:00         |

### Adjustable Constants in Code

| Constant                      | Default       | Description                              |
|-------------------------------|---------------|------------------------------------------|
| `DEBOUNCE_BTN_TIME`           | 400 ms        | Button debounce delay                    |
| `EXIT_MENU_TIMEOUT`           | 20 seconds    | Exit menu automatically after inactivity |
| `LCD_BACKLIGHT_OFF_TIMOUT_MS` | 5 minutes     | Turn off LCD backlight after inactivity  |
| `LCD_CHARACTERS_LEN`          | 16            | LCD width (characters per line)          |

## How the Lamp Scheduling Works

The `calculateLigthState()` function determines whether the lamp should be on or off based on the current time and the configured schedule. It handles three scenarios:

1. **Same hour for on and off** : Uses minutes to determine state
2. **Turn off hour is after turn on hour** (e.g., 09:00 to 18:00) : Standard daytime schedule
3. **Turn off hour is before turn on hour** (e.g., 22:00 to 06:00) : Overnight schedule spanning midnight

## Troubleshooting

| Problem | Solution |
|---------|----------|
| LCD shows nothing | Check I2C address (try 0x3F if 0x27 doesn't work). Verify wiring. |
| Time resets on power off | Ensure the RTC module has a working battery (CR2032). |
| Buttons not responding | Check wiring. Ensure buttons connect to GND when pressed. |
| Lamp doesn't switch | Verify pin 13 output with the onboard LED first. Check relay wiring. |

## License

This project is open source. Feel free to modify and distribute.

## Acknowledgments

- YWROBOT for the original LCD library example
- Makuna for the RtcDS1302 library
