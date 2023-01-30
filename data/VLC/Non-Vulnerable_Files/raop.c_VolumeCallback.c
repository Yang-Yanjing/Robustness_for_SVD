 *****************************************************************************/
static int VolumeCallback( vlc_object_t *p_this, char const *psz_cmd,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(psz_cmd);
    VLC_UNUSED(oldval);
    VLC_UNUSED(p_data);
    VLC_UNUSED(newval);
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* TODO: Implement volume change */
    VLC_UNUSED(p_sys);
    return VLC_SUCCESS;
}
