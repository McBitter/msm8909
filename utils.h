#ifndef UTILS_H
#define UTILS_H
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "termios.h"
#include "errno.h"

#define BUFFER_SIZE 4096
#define INIT_READ 1024

enum
{
    COMM_CHIP_HELLO  = 0x01,
    COMM_ANSW_HELLO  = 0x02,
    
    COMM_READ_DATA   = 0x03,
    COMM_READ_DATA64 = 0x12, // unused

    COMM_WRITE_DATA  = 0x04,

    COMM_DONE_DATA   = 0x05,
    COMM_DONE_ANSWER = 0x06,
    
    COMM_MEM_DEBUG64 = 0x10, // unused
    COMM_MEM_DEBUG   = 0x09, // unused
};

struct PacketHeader
{
    uint32_t command;
    uint32_t wholePacketSize;

    union
    {
	char data[BUFFER_SIZE - 8];

	// used to parse SoC packet
	struct
	{
	    uint32_t unk_0; // almost always 0x0D
	    uint32_t offsetToProgram; // seems to increase by size of recv buffer
	    uint32_t sizeOfRecvBuffer; // on the device
	};
    };
};

int set_serial_attribs(int fd, int speed);
void printBuffer(void* bufferToPrint, ssize_t size);
void clearBuffer(void* bufferToClear);
void answerHello(struct PacketHeader* bufferToEdit);


#endif // UTILS_H
