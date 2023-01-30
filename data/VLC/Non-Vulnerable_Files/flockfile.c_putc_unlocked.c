}
int putc_unlocked (int c, FILE *stream)
{
    return _putc_nolock (c, stream);
}
