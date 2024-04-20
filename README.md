# dueATC

Another implementation for MB 722.6 / 5GTronic automatic transmission control unit, running on Arduino Due.

First version made only for my own use has been running on Arduino Mega since 2017; see https://www.youtube.com/watch?v=6ptkRtwG5Mg

Chassis is a Volvo 940 STW from 1993. Transmission has a welded bellhousing to fit a 2.5TDI VAG engine.

My goal is to create a rather simple, easy-to-approach and versatile open-source controller, which you should be able to customize to your own needs just with Arduino and basic OOP skills.
Both hardware and software are still in progress. PCB design is ~90 % complete, so far I've made only lab-testing with breadboards etc.
The controller will have and 128x64 LCD screen ("U8g2" library), SD-card reader ("SD" library), and a button pad. Also library "DueTimer" is utilized.

I have no deadline for the project, and the main target is to satisfy my own needs, so probably this will not ever be a product I could sell. But you never know.

Sorry for the Finnish language comments left in the code. :D

-Tuomas

UPDATE 20.4.2024:
Finally the PCB rev 0.4 got to be mature enough to test it in the car - see;
https://www.youtube.com/watch?v=vycVulFN1EY