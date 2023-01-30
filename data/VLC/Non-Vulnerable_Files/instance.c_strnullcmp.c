/** Compares two string pointers that might be NULL */
static int strnullcmp(const char *a, const char *b)
{
    if (b == NULL)
        return a != NULL;
    if (a == NULL)
        return -1;
    return strcmp(a, b);
}
