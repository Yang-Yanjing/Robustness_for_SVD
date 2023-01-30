static void *GetSymbol(vlc_gl_t *, const char *);
static bool CheckToken(const char *haystack, const char *needle)
{
    size_t len = strlen(needle);
    while (haystack != NULL)
    {
        while (*haystack == ' ')
            haystack++;
        if (!strncmp(haystack, needle, len)
         && (memchr(" ", haystack[len], 2) != NULL))
            return true;
        haystack = strchr(haystack, ' ');
    }
    return false;
}
