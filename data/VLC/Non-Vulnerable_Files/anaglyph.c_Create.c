};
static int Create(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    switch (p_filter->fmt_in.video.i_chroma)
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
            break;
        default:
            msg_Err(p_filter, "Unsupported input chroma (%4.4s)",
                    (char*)&(p_filter->fmt_in.video.i_chroma));
            return VLC_EGENERIC;
    }
    p_filter->p_sys = malloc(sizeof(filter_sys_t));
    if (!p_filter->p_sys)
        return VLC_ENOMEM;
    filter_sys_t *p_sys = p_filter->p_sys;
    config_ChainParse(p_filter, FILTER_PREFIX, ppsz_filter_options,
                      p_filter->p_cfg);
    char *psz_scheme = var_CreateGetStringCommand(p_filter,
                                                  FILTER_PREFIX "scheme");
    enum scheme_e scheme = red_cyan;
    if (psz_scheme)
    {
        if (!strcmp(psz_scheme, "red-green"))
            scheme = red_green;
        else if (!strcmp(psz_scheme, "red-blue"))
            scheme = red_blue;
        else if (!strcmp(psz_scheme, "red-cyan"))
            scheme = red_cyan;
        else if (!strcmp(psz_scheme, "trioscopic"))
            scheme = trioscopic;
        else if (!strcmp(psz_scheme, "magenta-cyan"))
            scheme = magenta_cyan;
        else
            msg_Err(p_filter, "Unknown anaglyph color scheme '%s'", psz_scheme);
    }
    free(psz_scheme);
    switch (scheme)
    {
        case red_green:
            p_sys->left = 0xff0000;
            p_sys->right = 0x00ff00;
            break;
        case red_blue:
            p_sys->left = 0xff0000;
            p_sys->right = 0x0000ff;
            break;
        case red_cyan:
            p_sys->left = 0xff0000;
            p_sys->right = 0x00ffff;
            break;
        case trioscopic:
            p_sys->left = 0x00ff00;
            p_sys->right = 0xff00ff;
            break;
        case magenta_cyan:
            p_sys->left = 0xff00ff;
            p_sys->right = 0x00ffff;
            break;
        case unknown:
            msg_Err(p_filter, "Oops");
            break;
    }
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
