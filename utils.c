#include "utils.h"

int set_serial_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0)
    {
	printf("Failed to get tcgetattr: %s\n", strerror(errno));
	return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
	printf("Error from tcsetattr: %s\n", strerror(errno));
	return -1;
    }

    return 0;
}

void printBuffer(void* bufferToPrint, ssize_t size)
{
    for (int i = 0; i < size; i++)
	printf("%2X ", ((char*)bufferToPrint)[i]);
    
    printf("\n");
}

void clearBuffer(void* bufferToClear)
{
        memset(bufferToClear, '0', BUFFER_SIZE);
}

void answerHello(struct PacketHeader *bufferToEdit)
{
    bufferToEdit->command = COMM_ANSW_HELLO; //0
    // might be some future mode setter...
    *(bufferToEdit->data + 9) = 0x0; // answer packet just resets this
}
