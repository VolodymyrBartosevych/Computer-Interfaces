# Serial Port Communication Tool

A Windows-based C++ application for serial communication via RS-232C interface. This allows you to send and receive text data, text files, and binary files through serial ports.

## Features
- **Text Communication**: Send and receive text strings
- **File Transfer**: Send and receive text files
- **Binary File Transfer**: Send and receive binary files (images, executables, etc.)

## Supported Configuration Options

### Serial Ports
- COM1 through COM10

### Baud Rates
- 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200, 128000, 256000

### Parity Options
- No Parity (default)
- Odd Parity

### Stop Bits
- One Stop Bit (default)
- Two Stop Bits

### Byte Size
- 5, 6, 7, 8 bits (default: 8)

## Requirements

- Windows operating system
- C++ compiler with Windows API support (Visual Studio, MinGW, etc.)
- Available serial port (physical or virtual)

## Usage

### Running the Application

To run the application, compile it with:
```bash
g++ serial.cpp -o serial.exe
```
Then run:
```bash
./serial.exe
```

