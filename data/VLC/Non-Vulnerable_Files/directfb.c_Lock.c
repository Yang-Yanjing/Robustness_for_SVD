};
static int Lock(picture_t *pic)
{
    vout_display_sys_t *sys = pic->p_sys->sys;
    return sys->pics[sys->idx] == pic ? VLC_SUCCESS : VLC_EGENERIC;
}
