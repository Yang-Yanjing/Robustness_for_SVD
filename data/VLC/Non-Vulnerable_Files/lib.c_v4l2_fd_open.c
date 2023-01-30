}
int v4l2_fd_open (int fd, int flags)
{
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once (&once, v4l2_lib_load);
    return v4l2_fd_open_ (fd, flags);
}
