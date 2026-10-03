#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int openSerialPort(const char* portName) {
    int serialFd = open(portName, O_RDWR | O_NOCTTY);
    struct termios tty;
    tcgetattr(serialFd, &tty);
    
    cfsetispeed(&tty, B9600); // Set baud rate to match microcontroller
    cfsetospeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver
    tcsetattr(serialFd, TCSANOW, &tty);
    
    return serialFd;
}