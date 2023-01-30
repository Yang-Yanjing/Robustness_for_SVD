}
static int sendKeyEvent(demux_sys_t *p_sys, unsigned int key)
{
    mtime_t now = mdate();
    if (bd_user_input(p_sys->bluray, now, key) < 0)
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
