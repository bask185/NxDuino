#!/bin/bash
python.exe updateTimers.py
python.exe updateIO.py
echo "COMPILING"
arduino-cli.exe compile -b arduino:avr:nano ~/Documents/software/NxDuino -e
echo "UPLOADING"
arduino-cli.exe upload -b arduino:avr:nano:cpu=atmega328old -p COM3 -i ~/Documents/software/NxDuino/build/arduino.avr.nano/NxDuino.ino.with_bootloader.hex
rm -r builds/arduino.avr.nano/*

exit