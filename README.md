# half-life-time-tracker
Création de mon premier projet HackClub Half-Life.


A small ESP32-based device designed to track the time I spend working on my projects.

The goal is to have a simple physical timer that I can use while working on my Half Life projects, without having to constantly check my computer or phone.

## Goal

The Time Tracker will allow me to:

* Start a work session with a button
* Display the elapsed time on a 4-digit 7-segment display
* Remind me to take a photo every hour
* Keep counting while the photo reminder is displayed
* End the current session with a long press
* Start a new session afterwards
* Eventually save my session time so it is not lost when the device is turned off

## How it will work

The ESP32 is the main microcontroller of the project. It will handle the timer, button inputs and the display.

The planned controls are:

| Action                       | Function                          |
| ---------------------------- | --------------------------------- |
| Short press                  | Start the timer                   |
| Short press during a session | Dismiss the hourly photo reminder |
| Long press                   | End the current session           |

Every hour, the display will blink to remind me to take a photo for my project journal.

The timer itself will continue running in the background while the display is blinking.

## Hardware

The first version will be built on a breadboard.

Main components:

* ESP32 DevKit
* 3641BS 4-digit 7-segment display
* Push button
* Resistors
* Breadboard
* Jumper wires
* USB power
* register 74HC595

A 3D-printed enclosure may be designed later once the electronics and software are working.

## 🧪 Development Plan

### 1. Breadboard prototype

* Connect the ESP32
* Connect the 3641BS display
* Add the necessary resistors
* Connect the push button
* Test each component separately

### 2. Timer software

* Display elapsed time
* Detect short presses
* Detect long presses
* Start and stop sessions
* Add the hourly reminder

### 3. Persistence

* Save relevant timing information
* Make sure the data is not accidentally lost when the device is restarted

### 4. Enclosure

* Design a 3D-printed case
* Make sure the display and button are accessible
* Make the electronics fit securely inside

### 5. Final version

* Assemble the complete device
* Test it during real work sessions
* Document the final build

## 🚧 Current Status

**Project started — October 1, 2026**

Current progress:

* [x] Project created
* [x] GitHub repository created
* [x] Initial components identified
* [ ] Connect the 3641BS to the ESP32
* [ ] Test the display
* [ ] Test the button
* [ ] Program the timer
* [ ] Add hourly photo reminders
* [ ] Add session persistence
* [ ] Design the enclosure
* [ ] Assemble the final version
* [ ] Test the complete device

## 🔮 Possible Future Improvements

Depending on how the first version goes, I may add:

* Rechargeable battery power
* Better power management
* A physical power switch
* A more compact PCB
* A custom 3D-printed enclosure
* Improved display animations
