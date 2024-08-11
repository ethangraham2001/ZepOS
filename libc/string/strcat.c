#include <string.h>

char *strcat(char *dest, const char *src)
{
	unsigned int dest_len = strlen(dest);
	unsigned int src_len = strlen(src);
	for (size_t i = 0; i < src_len; i++)
		dest[i + dest_len] = src[i];
	return dest;
}
