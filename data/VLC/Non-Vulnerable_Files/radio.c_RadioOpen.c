}
int RadioOpen (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    /* Parse MRL */
    size_t pathlen = strcspn (demux->psz_location, ":;");
    char *path = (pathlen != 0) ? strndup (demux->psz_location, pathlen)
                              : var_InheritString (obj, CFG_PREFIX"radio-dev");
    if (unlikely(path == NULL))
        return VLC_ENOMEM;
    if (demux->psz_location[pathlen] != '\0')
        var_LocationParse (obj, demux->psz_location + pathlen + 1, CFG_PREFIX);
    /* Open device */
    uint32_t caps;
    int fd = OpenDevice (obj, path, &caps);
    free (path);
    if (fd == -1)
        return VLC_EGENERIC;
    if (!(caps & V4L2_CAP_TUNER))
    {
        msg_Err (obj, "not a radio tuner device");
        goto error;
    }
    if (SetupTuner (obj, fd, 0))
        goto error;
    demux_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        goto error;
    sys->fd = fd;
    sys->controls = ControlsInit (VLC_OBJECT(demux), fd);
    sys->start = mdate ();
    demux->p_sys = sys;
    demux->pf_demux = NULL;
    demux->pf_control = RadioControl;
    demux->info.i_update = 0;
    demux->info.i_title = 0;
    demux->info.i_seekpoint = 0;
    return VLC_SUCCESS;
error:
    v4l2_close (fd);
    return VLC_EGENERIC;
}
