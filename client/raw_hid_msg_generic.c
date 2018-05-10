#include <stdio.h>
#include <string.h>

#include "hid.h"

int main(int argc, char **argv)
{
	char hid_buffer[64];
	if (argc < 2) {
		fprintf(stderr, "Usage %s <message>\n", argv[0]);
		return -1;
	}
	int count = rawhid_open(1, 0x5E2A /* VID */ , 1 /* PID */, 0xffab /* usage page */, 0x200 /* USAGE */);
	if (count != 1) {
		fprintf(stderr, "Couldn't open device\n");
	}
	if (strlen(argv[1]) > 63)
		argv[63] = 0;
	strcpy(hid_buffer, argv[1]);
	
	rawhid_send(0, hid_buffer, sizeof(hid_buffer), 1000);

	rawhid_close(0);
}
