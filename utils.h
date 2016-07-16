#ifndef UTILS_H
#define UTILS_H
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "termios.h"
#include "errno.h"

#define BUFFER_SIZE 4096

enum
{
    COMM_CHIP_HELLO  = 0x01,
    COMM_ANSW_HELLO  = 0x02,
    
    COMM_READ_DATA   = 0x03,
    COMM_READ_DATA64 = 0x12, // unused

    COMM_MEM_DEBUG64 = 0x10, // unused
    COMM_MEM_DEBUG   = 0x09, // unused
};

struct PacketHeader
{
    uint32_t command;
    uint32_t wholePacketSize;
    char data[BUFFER_SIZE - 8];
};

int set_serial_attribs(int fd, int speed);
void printBuffer(void* bufferToPrint, ssize_t size);
void clearBuffer(void* bufferToClear);
void answerHello(struct PacketHeader* bufferToEdit);

#endif // UTILS_H
