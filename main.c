#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"

#include "utils.h"


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

    set_serial_attribs(fd, B115200);
    
    struct PacketHeader* recvBuffer = malloc(BUFFER_SIZE);

    if (recvBuffer == NULL)
    {
	printf("Failed to allocate memory... exiting");
	close(fd);
	return -1;
    }
    
    struct PacketHeader* sendBuffer = malloc(BUFFER_SIZE);

    if (sendBuffer == NULL)
    {
	printf("Failed to allocate memory... exiting");
	free(recvBuffer);
	close(fd);
	return -1;
    }

    clearBuffer(recvBuffer);
    clearBuffer(sendBuffer);

    int bytesRead = 0;
    int bytesReadtmp = 0;
    int actualBytes = 0;

runner:
 
    bytesRead = read(fd, recvBuffer, 1024); // some kind of limit
    
    printf("Bytes from packet field: %d\n", recvBuffer->wholePacketSize);
    
    if (bytesRead < actualBytes)
	printf("Still failed to get all bytes... contiuning still...\n");
	
    printf("Bytes read: %d\n", bytesRead);

    // data in little endian

    switch (recvBuffer->command)
    {
    case COMM_CHIP_HELLO:
    {
	printBuffer(recvBuffer, bytesRead);
	printf("Hello recvd\n");
	answerHello(recvBuffer);
	int bytesWritten = write(fd, recvBuffer, bytesRead);

	if (bytesWritten > 0)
	{
	    printf("Writing to device:\n");
	    printBuffer(recvBuffer, bytesWritten);
	}
	break;
    }
    default:
	printf("Nothing happened...\n");
	printBuffer(recvBuffer, bytesRead);
	goto end;
	break;
    }

    goto runner;
    
end:
    close(fd);
    return 0;
}

