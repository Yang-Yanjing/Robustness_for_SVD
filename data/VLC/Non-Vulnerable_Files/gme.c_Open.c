static gme_err_t ReaderBlock (void *, void *, int);
static int Open (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    int64_t size = stream_Size (demux->s);
    if (size > LONG_MAX /* too big for GME */)
        return VLC_EGENERIC;
    /* Auto detection */
    const uint8_t *peek;
    if (stream_Peek (demux->s, &peek, 4) < 4)
        return VLC_EGENERIC;
    const char *type = gme_identify_header (peek);
    if (!*type)
        return VLC_EGENERIC;
    msg_Dbg (obj, "detected file type %s", type);
    block_t *data = NULL;
    if (size <= 0)
    {
        data = stream_Block (demux->s, 1 << 24);
        if (data == NULL)
            return VLC_EGENERIC;
    }
    /* Initialization */
    demux_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->emu = gme_new_emu (gme_identify_extension (type), RATE);
    if (sys->emu == NULL)
    {
        free (sys);
        return VLC_ENOMEM;
    }
    if (data)
    {
        gme_load_custom (sys->emu, ReaderBlock, data->i_buffer, data);
        block_Release(data);
    }
    else
    {
        gme_load_custom (sys->emu, ReaderStream, size, demux->s);
    }
    gme_start_track (sys->emu, sys->track_id = 0);
    es_format_t fmt;
    es_format_Init (&fmt, AUDIO_ES, VLC_CODEC_S16N);
    fmt.audio.i_rate = RATE;
    fmt.audio.i_bytes_per_frame = 4;
    fmt.audio.i_frame_length = 4;
    fmt.audio.i_channels = 2;
    fmt.audio.i_blockalign = 4;
    fmt.audio.i_bitspersample = 16;
    fmt.i_bitrate = RATE * 4;
    sys->es = es_out_Add (demux->out, &fmt);
    date_Init (&sys->pts, RATE, 1);
    date_Set (&sys->pts, 0);
    /* Titles */
    unsigned n = gme_track_count (sys->emu);
    sys->titlev = malloc (n * sizeof (*sys->titlev));
    if (unlikely(sys->titlev == NULL))
        n = 0;
    sys->titlec = n;
    for (unsigned i = 0; i < n; i++)
    {
         input_title_t *title = vlc_input_title_New ();
         sys->titlev[i] = title;
         if (unlikely(title == NULL))
             continue;
         gme_info_t *infos;
         if (gme_track_info (sys->emu, &infos, i))
             continue;
         msg_Dbg (obj, "track %u: %s %d ms", i, infos->song, infos->length);
         if (infos->length != -1)
             title->i_length = infos->length * INT64_C(1000);
         if (infos->song[0])
             title->psz_name = strdup (infos->song);
         gme_free_info (infos);
    }
    /* Callbacks */
    demux->pf_demux = Demux;
    demux->pf_control = Control;
    demux->p_sys = sys;
    return VLC_SUCCESS;
}
