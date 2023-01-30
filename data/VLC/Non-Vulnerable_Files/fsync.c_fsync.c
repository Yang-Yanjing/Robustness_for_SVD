#endif
int fsync (int fd)
{
#if defined(_WIN32)
    /* WinCE can use FlushFileBuffers() but it operates on file handles */
    return _commit (fd);
#else
# warning fsync() not implemented!
    return 0;
#endif
}
