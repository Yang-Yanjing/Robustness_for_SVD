};
static int Open (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    demux_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->detach = no_detach;
    uint32_t chroma;
    uint16_t width = 0, height = 0;
    uint8_t bpp;
    switch (var_InheritInteger (demux, "shm-depth"))
    {
        case 32:
            chroma = VLC_CODEC_RGB32; bpp = 32;
            break;
        case 24:
            chroma = VLC_CODEC_RGB24; bpp = 24;
            break;
        case 16:
            chroma = VLC_CODEC_RGB16; bpp = 16;
            break;
        case 15:
            chroma = VLC_CODEC_RGB15; bpp = 16;
            break;
        case 8:
            chroma = VLC_CODEC_RGB8; bpp = 8;
            break;
        case 0:
            chroma = VLC_CODEC_XWD; bpp = 0;
            break;
        default:
            goto error;
    }
    if (bpp != 0)
    {
        width = var_InheritInteger (demux, "shm-width");
        height = var_InheritInteger (demux, "shm-height");
    }
    static void (*Demux) (void *);
    char *path = var_InheritString (demux, "shm-file");
    if (path != NULL)
    {
        sys->fd = vlc_open (path, O_RDONLY);
        if (sys->fd == -1)
            msg_Err (demux, "cannot open file %s: %s", path,
                     vlc_strerror_c(errno));
        free (path);
        if (sys->fd == -1)
            goto error;
        sys->detach = CloseFile;
        Demux = DemuxFile;
    }
    else
    {
#ifdef HAVE_SYS_SHM_H
        sys->mem.length = width * height * (bpp >> 3);
        if (sys->mem.length == 0)
            goto error;
        int id = var_InheritInteger (demux, "shm-id");
        if (id == IPC_PRIVATE)
            goto error;
        void *mem = shmat (id, NULL, SHM_RDONLY);
        if (mem == (const void *)(-1))
        {
            msg_Err (demux, "cannot attach segment %d: %s", id,
                     vlc_strerror_c(errno));
            goto error;
        }
        sys->mem.addr = mem;
        sys->detach = CloseIPC;
        Demux = DemuxIPC;
#else
        goto error;
#endif
    }
    /* Initializes format */
    float rate = var_InheritFloat (obj, "shm-fps");
    if (rate <= 0.)
        goto error;
    mtime_t interval = llroundf((float)CLOCK_FREQ / rate);
    if (!interval)
        goto error;
    es_format_t fmt;
    es_format_Init (&fmt, VIDEO_ES, chroma);
    fmt.video.i_chroma = chroma;
    fmt.video.i_bits_per_pixel = bpp;
    fmt.video.i_sar_num = fmt.video.i_sar_den = 1;
    fmt.video.i_frame_rate = 1000 * rate;
    fmt.video.i_frame_rate_base = 1000;
    fmt.video.i_visible_width = fmt.video.i_width = width;
    fmt.video.i_visible_height = fmt.video.i_height = height;
    sys->es = es_out_Add (demux->out, &fmt);
    /* Initializes demux */
    if (vlc_timer_create (&sys->timer, Demux, demux))
        goto error;
    vlc_timer_schedule (sys->timer, false, 1, interval);
    demux->p_sys = sys;
    demux->pf_demux   = NULL;
    demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    sys->detach (sys);
    free (sys);
    return VLC_EGENERIC;
}
