# MotoNunchuk

##Prerequisites
* ncurses
`sudo apt install libncurses-dev`
* I2C enabled in Raspberry (run *sudo raspi-config* and enable I2C)

##Compiling:

`gcc -o MotoNunchuk ./MotoNunchuk.c -I/usr/local/include -L/usr/local/lib -lwiringPi -lncurses`
