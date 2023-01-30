 *****************************************************************************/
static int onMouseEvent(vlc_object_t *p_vout, const char *psz_var, vlc_value_t old,
                        vlc_value_t val, void *p_data)
{
    demux_t     *p_demux = (demux_t*)p_data;
    demux_sys_t *p_sys   = p_demux->p_sys;
    mtime_t     now      = mdate();
    VLC_UNUSED(old);
    VLC_UNUSED(p_vout);
    if (psz_var[6] == 'm')   //Mouse moved
        bd_mouse_select(p_sys->bluray, now, val.coords.x, val.coords.y);
    else if (psz_var[6] == 'c') {
        bd_mouse_select(p_sys->bluray, now, val.coords.x, val.coords.y);
        bd_user_input(p_sys->bluray, now, BD_VK_MOUSE_ACTIVATE);
    } else {
        assert(0);
    }
    return VLC_SUCCESS;
}
