}
void RadioClose (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    demux_sys_t *sys = demux->p_sys;
    ControlsDeinit (obj, sys->controls);
    v4l2_close (sys->fd);
    free (sys);
}
