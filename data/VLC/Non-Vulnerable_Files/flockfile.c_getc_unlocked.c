}
int getc_unlocked (FILE *stream)
{
    return _getc_nolock (stream);
}
