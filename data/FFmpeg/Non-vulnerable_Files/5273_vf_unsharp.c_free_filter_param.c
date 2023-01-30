static void free_filter_param(UnsharpFilterParam *fp)
{
    int z;
    for (z = 0; z < 2 * fp->steps_y; z++)
        av_freep(&fp->sc[z]);
}
