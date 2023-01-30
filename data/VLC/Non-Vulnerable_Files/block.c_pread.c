static
ssize_t pread (int fd, void *buf, size_t count, off_t offset)
{
    HANDLE handle = (HANDLE)(intptr_t)_get_osfhandle (fd);
    if (handle == INVALID_HANDLE_VALUE)
        return -1;
    OVERLAPPED olap; olap.Offset = offset; olap.OffsetHigh = (offset >> 32);
    DWORD written;
    /* This braindead API will override the file pointer even if we specify
     * an explicit read offset... So do not expect this to mix well with
     * regular read() calls. */
    if (ReadFile (handle, buf, count, &written, &olap))
        return written;
    return -1;
}
