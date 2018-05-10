#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "hid.h"

int main(int argc, char **argv)
{
	char hid_buffer[64 + 1];
	if (argc < 3) {
		fprintf(stderr, "Usage %s <device file> <message>\n", argv[0]);
		return -1;
	}
	int count = rawhid_open(1, 0x5E2A /* VID */ , 1 /* PID */, 0xffab /* usage page */, 0x200 /* USAGE */);
	if (count != 1) {
		fprintf(stderr, "Couldn't open device\n");
	
	hid_buffer[0] = 0;
	if (strlen(argv[2]) > 63)
		argv[63] = 0;
	strcpy(hid_buffer + 1, argv[2]);
	
	rawhid_send(0, hid_buffer, sizeof(hid_buffer), 1000);

	rawhid_close(0);
}
