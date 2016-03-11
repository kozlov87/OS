#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
char buf[4096];
int main()
{
	ssize_t bytes_read = 0,  bytes_write = 0;
	while (1)
	{
		bytes_read = read(STDIN_FILENO, buf, sizeof(buf));
		if (bytes_read == 0)
		{
			return 0;
		}
		else if (bytes_read < 0)
		{
			fprintf(stderr, "%s\n", "It's not bytes yet");
			return -1;
		}
		bytes_write = write(STDOUT_FILENO, buf, bytes_read);
		if (bytes_write < 0)
		{
			fprintf(stderr, "%s\n", "I can't write this");
			return -1;
		}	
	}
	return 0;
}
