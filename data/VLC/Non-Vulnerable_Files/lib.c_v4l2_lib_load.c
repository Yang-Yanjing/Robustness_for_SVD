}
static void v4l2_lib_load (void)
{
    void *h;
    h = dlopen ("libmediaclient.so", RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
    if (h == NULL)
        h = dlopen ("libv4l2.so.0", RTLD_LAZY | RTLD_LOCAL);
    if (h == NULL)
        goto fallback;
    v4l2_fd_open_ = dlsym (h, "v4l2_fd_open");
    v4l2_close = dlsym (h, "v4l2_close");
    v4l2_ioctl = dlsym (h, "v4l2_ioctl");
    v4l2_read = dlsym (h, "v4l2_read");
    v4l2_mmap = dlsym (h, "v4l2_mmap");
    v4l2_munmap = dlsym (h, "v4l2_munmap");
    if (v4l2_fd_open_ != NULL && v4l2_close != NULL && v4l2_ioctl != NULL
     && v4l2_read != NULL && v4l2_mmap != NULL && v4l2_munmap != NULL)
    {
        v4l2_handle = h;
        return;
    }
    dlclose (h);
fallback:
    v4l2_fd_open_ = fd_open;
    v4l2_close = close;
    v4l2_ioctl = ioctl;
    v4l2_read = read;
    v4l2_mmap = mmap;
    v4l2_munmap = munmap;
}
