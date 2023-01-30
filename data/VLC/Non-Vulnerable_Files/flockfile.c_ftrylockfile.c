}
int ftrylockfile (FILE *stream)
{
    flockfile (stream); /* Move along people, there is nothing to see here. */
    return 0;
}
