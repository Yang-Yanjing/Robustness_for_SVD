}
static int FixRV12 (video_format_t *fmt)
{
#ifndef WORDS_BIGENDIAN
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0x0f00;
        fmt->i_gmask = 0x00f0;
        fmt->i_bmask = 0x000f;
    }
    return (fmt->i_rmask == 0x0f00 && fmt->i_gmask == 0x00f0
         && fmt->i_bmask == 0x000f) ? 0 : -1;
#else
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0x000f;
        fmt->i_gmask = 0xf000;
        fmt->i_bmask = 0x0f00;
    }
    return (fmt->i_rmask == 0x000f && fmt->i_gmask == 0xf000
         && fmt->i_bmask == 0x0f00) ? 0 : -1;
#endif
}
