# Instant-Messenger
CMPUT 274 Final Project: Instant Messenger between two arduino megas.

Accessories:
2 x Arduino Mega 2560
2 x USB cable
2 x Adafruit 1.8” 18-bit color TFT LCD display with microSD card
2 x SparkFun COM-09032 – Thumb Joystick and breakout board
55 x wires

Wiring Instructions:
Single Arduino:
LCD Display:
- Arduino GND <--> Breadboard Negative Terminal <-->Adafruit LCD GND
- Arduino 5V <--> Breadboard Positive Terminal <--> Adafruit LCD VCC
- Arduino Pin 8 <--> Adafruit LCD RESET
- Arduino Pin 7 <--> Adafruit LCD D/C
- Arduino Pin 5 <--> Adafruit LCD CARD_CS
- Arduino Pin 6 <--> Adafruit LCD TFT_CS
- Arduino Pin 51 <--> Adafruit LCD MOSI
- Arduino Pin 52 <--> Adafruit LCD SCK
- Arduino Pin 50 <--> Adafruit LCD MISO
- Breadboard Positive Terminal <--> Adafruit LCD LITE

Joystick:
- Breadboard Positive Terminal <--> SparkFun Thumb Joystick VCC
- Arduino Pin A0 <--> SparkFun Thumb Joystick VERT
- Arduino Pin A1 <--> SparkFun Thumb Joystick HOR
- Arduino Pin 9 <--> SparkFun Thumb Joystick SEL
- Breadboard Negative Terminal <--> SparkFun Thumb Joystick GND

Between Two Arduinos:
Arduino 1:
-Arduino GND <--> Arduino GND (opposite arduino)
-TX3 <--> RX3 (opposite arduino)

Arduino 2:
-Arduino GND <--> Arduino GND (opposite arduino)
-RX3 <--> TX3 (opposite arduino)

Running the Code:
1. Attach the USB cable from Arduino Mega 2560 to the computer.
2. Select "Board" Arduino Mega 2560 under Tools--> Board --> Arduino Mega 2560 or Mega ADK.
3. Select appropriate port.
4. Clear build-cli folder before each run by typing "rm -rf build-cli/" into the terminal while in the proper directory
5. Upload code by typing "make upload && serial-mon" into your terminal while being in the proper directory.

Using in Serial Monitor:
1. After sucessfully uploading the code to the Arduino Mega 2560, open your serial monitor by typing "serial-mon" into your terminal.
2. Type the message you want to send to the other Arduino. The message should be displayed in your writing box on your LCD Display.
3. Hit [ENTER] and the message will be displayed in the message box on both your LCD Display and the partner's LCD Display.
4. Press the joystick inwards to activate the discussion nav and move it up or down to scroll through your messages.

Rules: 
1) User is either navigating the messages already stored or writing new messages, they cannot be navigating messages while writing,
as if you are writing you want to be able to have a live response of incoming messages.
2) Messages must can consists of at most 84 characters
3) Only 2 messages are displayed on the screen at a time, in a page like fashion

References: 
- The wiring instructions are from the CMPUT 274 Tangible Computing reference guide Section 9 (http://ugweb.cs.ualberta.ca/~c274/web/ConcreteComputing/section/adafruit_lcd.htm)
- the makefile was taken from ~/arduino-ua/examples/Parrot directory.
- the typedef structs were referenced from the linked_list codes written in the Wednesday/Friday lecture of CMPUT 274, currently posted as sample codes on eClass. 
