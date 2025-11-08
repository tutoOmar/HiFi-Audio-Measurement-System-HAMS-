# HiFi-Audio-Measurement-System-HAMS-
Overview

HiFi-Audio Measurement System (HAMS) is a hardware–software platform for precision measurement of distortion and noise in high-fidelity audio equipment. The system is built around a redesigned PCB using the PCM4201 24-bit ADC, PCM5100A DAC, and the ATSAMD21G18A microcontroller. It implements external clocking, I2S digital audio interfaces, and optimized anti-aliasing filters.

The firmware was developed using MPLAB X IDE v6.25 and programmed through a MPLAB SNAP debugger. The platform integrates synchronous data acquisition, calibration routines, optional DAC-ADC loopback testing, and LabVIEW modules for visualization and automated analysis.

The goal of HAMS is to provide an open, accessible, and academically aligned measurement tool capable of approaching the performance of professional audio analyzers at significantly lower cost.

Features

24-bit high-performance audio acquisition (PCM4201).

Low-noise playback path using PCM5100A.

ATSAMD21G18A microcontroller with full I2S support.

External master clock, BCK, and FS synchronization.

Anti-aliasing and reconstruction filters designed for audio metrology.

LabVIEW modules for visualization, FFT analysis, distortion metrics, and automated workflows.

Optional loopback test mode for system verification.

Open-source hardware and firmware.

Development Environment

MPLAB X IDE: Version 6.25

Programmer/Debugger: MPLAB SNAP

Firmware Language: C

Software Tools: LabVIEW for measurement and visualization
