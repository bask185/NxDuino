# NxDuino
Software for an Arduino based NX type control panel.

This software is ment for the NxDuino control panel. It is an arduino based control panel which allows users to build an NX type control panel to control their model railway layout with.
Essentially you can control a variaty of switchtes and LEDs by pressing a start and a stop button. Further more the panel may receive external information such as occupance detectors.
This may be used to highlight a led or not.

The control panel calculates routes from start to end position every time  when two buttons are pressed. Occupied tracks will be considered as a wall and an alternative route will be searched.
A green LED will flash if a path is found. A red LED will flash if no path is found.