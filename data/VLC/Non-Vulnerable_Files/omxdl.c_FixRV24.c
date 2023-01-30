/*** Helpers ***/
static int FixRV24 (video_format_t *fmt)
{
#ifndef WORDS_BIGENDIAN
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0xff0000;
        fmt->i_gmask = 0x00ff00;
        fmt->i_bmask = 0x0000ff;
    }
    return (fmt->i_rmask == 0xff0000 && fmt->i_gmask == 0x00ff00
         && fmt->i_bmask == 0x0000ff) ? 0 : -1;
#else
    if (fmt->i_rmask == 0 && fmt->i_gmask == 0 && fmt->i_bmask == 0)
    {
        fmt->i_rmask = 0x0000ff;
        fmt->i_gmask = 0x00ff00;
        fmt->i_bmask = 0xff0000;
    }
    return (fmt->i_rmask == 0x0000ff && fmt->i_gmask == 0x00ff00
         && fmt->i_bmask == 0xff0000) ? 0 : -1;
#endif
}
