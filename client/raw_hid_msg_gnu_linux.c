#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	char hid_buffer[64 + 1];
	if (argc < 3) {
		fprintf(stderr, "Usage %s <device file> <message>\n", argv[0]);
		return -1;
	}
	int fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Couldn't open device file\n");
		return -1;
	}
	

	hid_buffer[0] = 0;
	if (strlen(argv[2]) > 63)
		argv[63] = 0;
	strcpy(hid_buffer + 1, argv[2]);
	
	write(fd, hid_buffer, sizeof(hid_buffer));

	close(fd);
}
