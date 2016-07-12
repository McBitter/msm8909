#include "stdio.h"
#include "stdlib.h"
#include "libusb.h"

#define BUFFER_SIZE 4096

int main()
{
    libusb_device **devs;
    libusb_device_handle *handle;
    libusb_context *ctx;
    
    int r = libusb_init(&ctx);

    if (r < 0)
    {
	libusb_error_name(r);
	goto cleanup;
    }

    libusb_set_debug(ctx, 3);
    ssize_t count = libusb_get_device_list(ctx, &devs);

    if (count < 0)
    {
	libusb_error_name(count);
	goto cleanup;
    }

    handle = libusb_open_device_with_vid_pid(ctx, 0x05c6, 0x9008);

    if (handle == NULL)
    {
	printf("Unable to find DLOAD capable device, sure it\'s bricked enough?\n");
	goto cleanup;
    }

    r = libusb_claim_interface(handle, 0);

    if (r < 0)
    {
	libusb_error_name(r);
	goto cleanup;
    }
    
    printf("Start Programming\n");

    int transferred;
    void *buffer = malloc(BUFFER_SIZE);
    r = libusb_bulk_transfer(handle, LIBUSB_ENDPOINT_IN, buffer, BUFFER_SIZE, &transferred, 10000);

    if (r < 0)
    {
	libusb_error_name(r);
	goto opened_cleanup;
    }

    printf("Bytes transf: %d", transferred);

opened_cleanup:
    libusb_release_interface(handle, 0);
    free(buffer);
cleanup:
    libusb_free_device_list(devs, 1);
    libusb_close(handle);
    libusb_exit(ctx);
    
    return 0;
}

