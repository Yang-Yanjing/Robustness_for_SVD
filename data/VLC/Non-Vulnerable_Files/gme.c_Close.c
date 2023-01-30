}
static void Close (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    demux_sys_t *sys = demux->p_sys;
    for (unsigned i = 0, n = sys->titlec; i < n; i++)
        vlc_input_title_Delete (sys->titlev[i]);
    free (sys->titlev);
    gme_delete (sys->emu);
    free (sys);
}
