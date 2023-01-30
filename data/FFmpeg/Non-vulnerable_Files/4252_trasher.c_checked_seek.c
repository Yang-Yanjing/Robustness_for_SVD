static void checked_seek(FILE *stream, int64_t offset, int whence)
{
    offset = fseek(stream, offset, whence);
    if (offset < 0) {
        fprintf(stderr, "seek failed\n");
        exit(1);
    }
}
