# ⚡ MonitorApp – Real-Time Industrial Current Monitoring

MonitorApp is a Windows desktop application developed in C++ using MFC, designed to monitor analog current from up to 4 industrial input channels in real time.  
Built around the MODBUS RTU protocol and serial communication (RS-485), it interfaces with ICP DAS modules (I-7017RC and I-7024L) through isolated RS-232/RS-485 converters (I-7520R).

This project was developed as a Bachelor’s thesis in Automation and Applied Informatics and integrates hardware communication, protocol implementation, graphical monitoring, and alerting in a single unified application.

---

## 🎯 Project Goals

- Monitor real-time electrical current values from industrial modules
- Build a full MFC GUI for visualization, logging, and control
- Implement MODBUS RTU communication from scratch (without external libraries)
- Support calibration, error simulation, and alarm thresholds
- Enable full operation with or without physical hardware (simulation mode)

---

## 🛠 Hardware Architecture

- **I-7017RC**: Analog input module (4 current channels, 4–20 mA)
- **I-7024L**: Analog output module (0–10V via DCON)
- **I-7520R**: RS-232 ↔ RS-485 isolated converters
- **PC**: Connects to both modules on separate COM ports

Each path is fully independent:
- Acquisition: `PC ↔ RS232 ↔ I-7520R ↔ I-7017RC (MODBUS RTU)`
- Generation: `PC ↔ RS232 ↔ I-7520R ↔ I-7024L (DCON Utility)`

---

## 💻 Key Features

### 🔧 Communication & Protocols
- Serial port configuration (COM, baud, parity, etc.)
- MODBUS RTU Master implementation (Function 04: Read Input Registers)
- CRC16 validation and error handling (timeout, address mismatch, invalid CRC)
- Dynamic slave ID configuration via GUI

### 📈 Data Acquisition & Display
- Real-time plotting of 4 current channels
- History slider to review past data
- Autoscaling Y-axis and channel toggle
- Instantaneous + average current per channel

### 🚨 Alerting & Logging
- Color-coded warnings and alarms (threshold + hysteresis)
- Peak detection logic (local signal maxima)
- Logging with timestamps (CSV export)
- Save chart as image (PNG)

### 🧪 Utilities & Simulation
- Two-point calibration (slope + offset)
- Full simulation mode (no hardware required)
- Dark Mode interface
- Help window with detailed function explanations

---

## 🧠 Technical Stack

| Component         | Technology                              |
|------------------|------------------------------------------|
| Language          | C++                                     |
| IDE               | Visual Studio                           |
| GUI               | MFC (Microsoft Foundation Classes)      |
| Communication     | Win32 Serial Port API                   |
| Protocol          | MODBUS RTU (custom, raw frame)          |
| OS Compatibility  | Windows (x86/x64)                       |

Modular code structure:
- SerialPort.cpp – port opening, reading, writing
- ModbusRTU.cpp – request framing, CRC16, response parsing
- MonitorAppDlg.cpp – main window, logic, chart
- CHelpDialog.cpp – user assistance pop-up

---

## 📸 Screenshots

> (insert screenshots or use links to images in /assets folder on GitHub)*

---

## 🧪 Example: MODBUS RTU Request

**Read Input Registers (Function 04)**  
Request frame:  
[SlaveID][Function][StartAddr_H][StartAddr_L][Qty_H][Qty_L][CRC_L][CRC_H]
01 04 00 00 00 04 XX XX

---

## 📚 Academic Context

- **Student**: Preda Robert-Constantin  
- **University**: Universitatea din Craiova  
- **Field**: Automation and Applied Informatics  
- **Coordinator**: Ș.l. dr. ing. Anca Albița  
- **Session**: July 2025  
- **Title**: Complex Monitoring Applications Using Industrial Modules from the I-7000/M-7000 Family

---

## 🚀 Future Improvements

- Mobile app version (Bluetooth over RS485 or MODBUS TCP)
- MODBUS TCP/IP support
- Email/Audio alerts on critical events
- Database integration for long-term logging

---

## 🧾 Quote

> "If you can't measure it, you can't understand it. If you can't understand it, you can't control it."  
> — Paraphrased from W. Thomson (Lord Kelvin)  

🎓 This principle guided the entire project.
