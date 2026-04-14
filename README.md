#  Real-Time Morse Code Decoder (AVR Embedded C)

A low-level embedded system designed to decode manual Morse code input into ASCII characters. This project was developed to master hardware-timed signal processing and asynchronous serial communication on AVR microcontrollers.

##  Project Goals & Learning Journey
This project started as a hobby experiment to understand how microcontrollers handle real-time human input. The main challenges addressed were:
- **Precision Timing:** Moving from simple `delay()` functions to hardware-based Timer/Counter interrupts.
- **Signal Logic:** Calibrating thresholds for dots and dashes to accommodate natural human clicking speed.
- **Communication:** Learning how to interface a bare-metal MCU with a PC terminal via USART.

##  Technical Specifications
- **Hardware:** AVR Series Microcontroller (running at 3.33 MHz).
- **Language:** Embedded C (No high-level libraries like Arduino).
- **Timer:** TCA0 (16-bit) configured with a **64 prescaler** for high-resolution pulse measurement.
- **Communication:** USART3 (Baud rate: 9600, 8N1).
- **Logic:** State-machine approach for non-blocking execution.

##  Engineering Features

### 1. Interrupt-Driven Pulse Measurement
Instead of polling, the system uses the **TCA0 overflow interrupt** to maintain a global micro-timer. This allows the CPU to handle other tasks while precisely measuring the duration of button presses.

### 2. Intelligent Space Detection
Implemented a **State-Flag logic (`space`)** to automatically detect the end of a word. The system intelligently sends a space character only after a character has been decoded and a specific idle threshold is met, preventing "ghost" spaces at startup.

### 3. Pattern Matching Algorithm
Decoupled the signal acquisition from the decoding logic. The system stores dots (0) and dashes (1) in a temporary buffer and compares them against a structured lookup table (struct-based alphabet) for efficient memory usage.

##  Hardware Setup
- **Input:** Push-button connected to **PIN F6** (Internal pull-up enabled).
- **Visual Feedback:** LED on **PIN F5** (Mashes Morse signal and system status).
- **Output:** USB-to-UART adapter connected to the PC.

##  Key Constants (Calibration)
Through testing and iteration, I calibrated the timing as follows:
- `DOT`: < 0.25s
- `DASH`: > 0.6s
- `GAP`: 1.2s (End of character)
- `WORD GAP`: ~3s (End of word/Space)

##  How to Use
1. Flash the code to your AVR MCU.
2. Open a Serial Terminal (like Putty or VS Code Serial Monitor) at 9600 Baud.
3. Start "tapping" Morse code.
4. Watch your words appear in the terminal!
