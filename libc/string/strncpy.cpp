#include <string.h>

char* strncpy(char* destination, const char* source, size_t num)
{
	// take a pointer pointing to the beginning of destination string
	char* ptr = destination;

	// copy first num characters of C-string pointed by source
	// into the array pointed by destination
	while (*source && num--)
	{
		*destination = *source;
		destination++;
		source++;
	}

	// null terminate destination string
	*destination = '\0';

	// destination is returned by standard strncpy()
	return ptr;
}