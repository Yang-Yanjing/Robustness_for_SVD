}
ssize_t getline (char **restrict lineptr, size_t *restrict n,
                 FILE *restrict stream)
{
    return getdelim (lineptr, n, '\n', stream);
}
