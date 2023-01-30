 */
static void Close (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    demux_sys_t *sys = demux->p_sys;
    vlc_timer_destroy (sys->timer);
    sys->detach (sys);
    free (sys);
}
