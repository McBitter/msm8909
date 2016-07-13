#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "errno.h"
#include "termios.h"
#include "fcntl.h"
#include "string.h"

#define BUFFER_SIZE 4096

int main()
{
    char* port = "/dev/ttyUSB0";
    int fd;
    int retryCount = 0;
retry:

    fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);

    if (fd < 0)
    {
	if (retryCount < 20)
	{
	    retryCount++;
	    sleep(1);
	    printf("Trying again...\n");
	    goto retry;
	}
	
	printf("Failed to open serial port %s: %s\n", port, strerror(errno));
	return -1;
    }

    void* recvBuffer = malloc(BUFFER_SIZE);

    if (recvBuffer == NULL)
    {
	printf("Failed to allocate memory... exiting");
	close(fd);
	return -1;
    }
    
    void* sendBuffer = malloc(BUFFER_SIZE);

    if (sendBuffer == NULL)
    {
	printf("Failed to allocate memory... exiting");
	close(fd);
	return -1;
    }

    memset(recvBuffer, '0', BUFFER_SIZE);
    memset(sendBuffer, '0', BUFFER_SIZE);

    int bytesRead = read(fd, recvBuffer, BUFFER_SIZE);

    if (bytesRead > 0)
    {
	for (int i = 0; i < bytesRead; i++)
	    printf("%2X ", ((char*)recvBuffer)[i]);

	printf("\n");
    }

    close(fd);
    return 0;
}

