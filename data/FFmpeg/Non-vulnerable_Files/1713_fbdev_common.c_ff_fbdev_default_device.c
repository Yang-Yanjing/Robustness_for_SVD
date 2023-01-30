const char* ff_fbdev_default_device()
{
    const char *dev = getenv("FRAMEBUFFER");
    if (!dev)
        dev = "/dev/fb0";
    return dev;
}
