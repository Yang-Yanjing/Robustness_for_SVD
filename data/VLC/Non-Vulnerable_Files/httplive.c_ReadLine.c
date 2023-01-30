}
static char *ReadLine(uint8_t *buffer, uint8_t **pos, const size_t len)
{
    assert(buffer);
    char *line = NULL;
    uint8_t *begin = buffer;
    uint8_t *p = begin;
    uint8_t *end = p + len;
    while (p < end)
    {
        if ((*p == '\r') || (*p == '\n') || (*p == '\0'))
            break;
        p++;
    }
    /* copy line excluding \r \n or \0 */
    line = strndup((char *)begin, p - begin);
    while ((*p == '\r') || (*p == '\n') || (*p == '\0'))
    {
        if (*p == '\0')
        {
            *pos = end;
            break;
        }
        else
        {
            /* next pass start after \r and \n */
            p++;
            *pos = p;
        }
    }
    return line;
}
