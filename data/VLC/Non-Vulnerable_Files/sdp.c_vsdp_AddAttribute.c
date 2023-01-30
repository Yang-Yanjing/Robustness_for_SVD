static char *
vsdp_AddAttribute (char **sdp, const char *name, const char *fmt, va_list ap)
{
    size_t oldlen = strlen (*sdp);
    size_t addlen = sizeof ("a=\r\n") + strlen (name);
    if (fmt != NULL)
    {
        va_list aq;
        va_copy (aq, ap);
        addlen += 1 + vsnprintf (NULL, 0, fmt, aq);
        va_end (aq);
    }
    char *ret = realloc (*sdp, oldlen + addlen);
    if (ret == NULL)
        return NULL;
    oldlen += sprintf (ret + oldlen, "a=%s", name);
    if (fmt != NULL)
    {
        ret[oldlen++] = ':';
        oldlen += vsprintf (ret + oldlen, fmt, ap);
    }
    strcpy (ret + oldlen, "\r\n");
    return *sdp = ret;
}
