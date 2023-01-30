 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    screen_CloseCapture( p_demux );
#ifdef SCREEN_MOUSE
    if( p_sys->p_mouse )
        picture_Release( p_sys->p_mouse );
#endif
    free( p_sys );
}
