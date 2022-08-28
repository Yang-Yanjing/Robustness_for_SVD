int main(int argc, char **argv)
{
    int w, h, i;
    char buf[1024];
    int isdir = 0;
    if (argc != 3) {
        printf("usage: %s image.pnm file|dir\n"
               "generate a test video stream\n", argv[0]);
        return 1;
    }
    if (!freopen(argv[2], "wb", stdout))
        isdir = 1;
    w = DEFAULT_WIDTH;
    h = DEFAULT_HEIGHT;
    rgb_tab = malloc(w * h * 3);
    wrap    = w * 3;
    width   = w;
    height  = h;
    if (init_demo(argv[1]))
        return 1;
    for (i = 0; i < DEFAULT_NB_PICT; i++) {
        gen_image(i, w, h);
        if (isdir) {
            snprintf(buf, sizeof(buf), "%s%02d.pgm", argv[2], i);
            pgmyuv_save(buf, w, h, rgb_tab);
        } else {
            pgmyuv_save(NULL, w, h, rgb_tab);
        }
    }
    free(rgb_tab);
    return 0;
}
