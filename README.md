# Universal Bus Converter 
## Description: 
Universal Bus Converter is a versatile hardware and software solution designed to facilitate communication between different protocols. It can convert data between CAN, Ethernet, I2C, and UART protocols, making it a valuable tool for various embedded systems.

## Hardware:
 #### *Microcontroller:* STM32F446RE
  The project is built around the STM32F446RE microcontroller, which serves as the central processing unit.
 #### *Peripheral Configuration:* STM32F4 HAL Library
  The STM32F4 HAL (Hardware Abstraction Layer) library is utilized to configure and control microcontroller peripherals, simplifying the development process.
 #### *Ethernet Interface:* ENC28J60 Module
  An external ENC28J60 module is integrated into the project for Ethernet communication. It is controlled using SPI to configure the MAC to enable communication with other devices over Ethernet.

  ## Software:
 #### *Programming Language:* C and C++
  The project is primarily developed using a combination of C and C++ languages. This allows for structured and object-oriented programming (OOP) to manage various communication protocols efficiently.
 #### *Design Pattern:* Observer Pattern
  The Observer Pattern is implemented to manage data reception and distribution. for example When data is received on the CAN bus, the Observer Pattern is used to notify all other communication protocols that are interested in the received CAN data.

  #### Protocol Support:
     1- CAN (Controller Area Network)
     2- Ethernet
     3- I2C (Inter-Integrated Circuit)
     4- UART (Universal Asynchronous Receiver-Transmitter)

  ## Tools and Utilities:
 #### Wireshark:
 Wireshark, a popular network protocol analyzer, is employed to monitor Ethernet packets between the universal bus converter and a connected PC. This facilitates debugging and analysis of network communication.
 #### Python Scapy Package:
The Python Scapy package is utilized to send Ethernet frames from a PC to the project's destination, which is the ENC28J60 module. This feature aids in testing and verifying the project's functionality.
