#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"

#include "utils.h"


int main()
{
    char* port = "/dev/ttyUSB0";
    
    char* ARMprogrammer = "./MPRG8909.mbn";
    int progFD;
    
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

    progFD = open(ARMprogrammer, O_RDONLY | O_RSYNC);

    if (progFD < 0)
    {
	printf("Failed to open file to program... %s: %s\n", ARMprogrammer, strerror(errno));
	return -1;
    }
    
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

runner:
 
    bytesRead = read(fd, recvBuffer, INIT_READ); // some kind of limit
    
    printf("Bytes from packet field: %d\n", recvBuffer->wholePacketSize);
    
    if (bytesRead < recvBuffer->wholePacketSize)
    {
	printf("Still failed to get all bytes... trying to get em...\n");
	int secondRead = read(fd, &recvBuffer->data[INIT_READ], recvBuffer->wholePacketSize - 1024);

	if (secondRead <= 0)
	    printf("Reading failed... report to developer about edge case");
	else
	    bytesRead += secondRead;
    }
	
    printf("Bytes read: %d\n", bytesRead);
    printBuffer(recvBuffer, bytesRead);
    
    // data in little endian
    switch (recvBuffer->command)
    {
    case COMM_CHIP_HELLO:
    {
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
    case COMM_READ_DATA:
    {
	printf("Read data recvd\n");

	if (recvBuffer->sizeOfRecvBuffer <= 0)
	{
	    printf("Recv buffer invalid...\n");
	    goto runner;
	}
	
	int bytesToWrite = read(progFD, sendBuffer, recvBuffer->sizeOfRecvBuffer);

	if (bytesToWrite < 0)
	{
	    printf("Failure to prepare data... is file still present?\n");
	    printf("Aborting...\n");
	    goto end;
	}
	else if (bytesToWrite == 0)
	{
	    printf("Soft fail... end of the file \n");
	}
	else
	{
	    int bytesWritten = write(fd, sendBuffer, bytesToWrite);

	    if (bytesWritten > 0)
		printf("Wrote: %d\n", bytesWritten);
	    else
	    {
		printf("Failed to write to device... aborting...\n");
		goto end;
	    }
	}
	break;
    }
    case COMM_DONE_DATA:
    {
	clearBuffer(sendBuffer);

	sendBuffer->command = COMM_DONE_ANSWER;
	sendBuffer->wholePacketSize = 8;
	
	int bytesWritten = write(fd, sendBuffer, 8);
	
	if (bytesWritten > 0)
	{
	    printf("Wrote: %d\n", bytesWritten);
	    printBuffer(sendBuffer, bytesWritten);
	}
	else
	{
	    printf("Failed to write to device... aborting...\n");
	    goto end;
	}
    }
    default:
	printf("Unhandled command recieved...\n");
	goto end;
	break;
    }
    
    goto runner;
    
end:
    close(fd);
    close(progFD);

    free(sendBuffer);
    free(recvBuffer);
    
    return 0;
}

