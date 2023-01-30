}
static int FixRV15 (video_format_t *fmt)
{
#ifndef WORDS_BIGENDIAN
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0x7c00;
        fmt->i_gmask = 0x03e0;
        fmt->i_bmask = 0x001f;
    }
    return (fmt->i_rmask == 0x7c00 && fmt->i_gmask == 0x03e0
         && fmt->i_bmask == 0x001f) ? 0 : -1;
#else
    (void) fmt;
    return -1;
#endif
}
