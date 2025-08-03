📦 Dispensing and Packing Machine – Arduino Project
This project contains the Arduino code for an automated dispensing and packing machine designed for small-scale industrial or retail use. The machine automates the process of measuring, dispensing, sealing, and packing, with precise motor control and integrated safety features.


🚀 Project Overview
The system is developed using Arduino Mega 2560 and includes the following core modules:

Dispensing Unit – Controls the flow of material (e.g., powder, grains) into the packet.

Weighing Unit – Monitors the packet weight in real-time using a load cell and HX711 amplifier.

Sealing Mechanism – Operates two heated sealers using stepper motors to seal the filled packet.

Bag Pulling Motor – A third stepper motor pulls the polythene bag through the machine to align it for each operation.

User Interface – Basic buttons or display (if applicable) for manual controls or feedback.


⚙️ Hardware Components
Arduino Mega 2560

3x NEMA 23 Stepper Motors

1x TB6560 Stepper Driver (for horizontal sealer)

2x TB6600 Stepper Drivers (for vertical sealer and dragging motor)

Load Cell + HX711 Amplifier

Sealing Bars

Relay modules (if needed)

Power Supply Unit (24V DC for motors)

Polythene roll (packing material)


🧠 Features
Automatic control of sealing and dispensing sequences

Weight-based dispensing using a load cell

Stepper motor-based sealing head movement

Bag advancement after each cycle

Modular code for easy debugging and testing

Safety stops and reset handling (optional)


🔌 How to Use
Flash individual .ino files to test each module separately.

For full system integration, upload full_automation.ino from the main_system folder.

Power the stepper motor drivers and Arduino board.

The full automation program will:

Start the dispensing process

Monitor the weight

Trigger the sealers

Pull the bag forward

Repeat or stop based on design


🛠️ Future Improvements
Add LCD or OLED display for real-time status

Add safety enclosures and emergency stop

Integrate a user-friendly control panel

Remote monitoring via IoT (Wi-Fi module or Bluetooth)



👨‍💻 Developed By
Team Novanex
Faculty of Information Technology, University of Moratuwa
