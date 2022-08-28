static void usage(void)
{
    printf("Show the relationships between rawvideo pixel formats and FourCC tags.\n");
    printf("usage: fourcc2pixfmt [OPTIONS]\n");
    printf("\n"
           "Options:\n"
           "-l                list the pixel format for each fourcc\n"
           "-L                list the fourccs for each pixel format\n"
           "-p PIX_FMT        given a pixel format, print the list of associated fourccs (one per line)\n"
           "-h                print this help\n");
}
