}
static int FixRV16 (video_format_t *fmt)
{
#ifndef WORDS_BIGENDIAN
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0xf800;
        fmt->i_gmask = 0x07e0;
        fmt->i_bmask = 0x001f;
    }
    return (fmt->i_rmask == 0xf800 && fmt->i_gmask == 0x07e0
         && fmt->i_bmask == 0x001f) ? 0 : -1;
#else
    (void) fmt;
    return -1;
#endif
}
