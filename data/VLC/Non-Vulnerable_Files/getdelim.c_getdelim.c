#include <stdlib.h>
ssize_t getdelim (char **restrict lineptr, size_t *restrict n,
                  int delimiter, FILE *restrict stream)
{
    char *ptr = *lineptr;
    size_t size = (ptr != NULL) ? *n : 0;
    size_t len = 0;
    for (;;)
    {
        if ((size - len) <= 2)
        {
            size = size ? (size * 2) : 256;
            ptr = realloc (*lineptr, size);
            if (ptr == NULL)
                return -1;
            *lineptr = ptr;
            *n = size;
        }
        int c = fgetc (stream);
        if (c == -1)
        {
            if (len == 0 || ferror (stream))
                return -1;
            break; /* EOF */
        }
        ptr[len++] = c;
        if (c == delimiter)
            break;
    }
    ptr[len] = '\0';
    return len;
}
