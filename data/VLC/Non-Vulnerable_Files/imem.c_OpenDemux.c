 */
static int OpenDemux(vlc_object_t *object)
{
    demux_t    *demux = (demux_t *)object;
    imem_sys_t *sys;
    if (OpenCommon(object, &sys, demux->psz_location))
        return VLC_EGENERIC;
    /* ES format */
    es_format_t fmt;
    es_format_Init(&fmt, UNKNOWN_ES, 0);
    fmt.i_id = var_InheritInteger(object, "imem-id");
    fmt.i_group = var_InheritInteger(object, "imem-group");
    char *tmp = var_InheritString(object, "imem-codec");
    if (tmp)
        fmt.i_codec = vlc_fourcc_GetCodecFromString(UNKNOWN_ES, tmp);
    free(tmp);
    const int cat = var_InheritInteger(object, "imem-cat");
    switch (cat) {
    case 1: {
        fmt.i_cat = AUDIO_ES;
        fmt.audio.i_channels = var_InheritInteger(object, "imem-channels");
        fmt.audio.i_rate = var_InheritInteger(object, "imem-samplerate");
        msg_Dbg(object, "Audio %4.4s %d channels %d Hz",
                (const char *)&fmt.i_codec,
                fmt.audio.i_channels, fmt.audio.i_rate);
        break;
    }
    case 2: {
        fmt.i_cat = VIDEO_ES;
        fmt.video.i_width  = var_InheritInteger(object, "imem-width");
        fmt.video.i_height = var_InheritInteger(object, "imem-height");
        unsigned num, den;
        if (!var_InheritURational(object, &num, &den, "imem-dar") && num > 0 && den > 0) {
            if (fmt.video.i_width > 0 && fmt.video.i_height > 0) {
                fmt.video.i_sar_num = num * fmt.video.i_height;
                fmt.video.i_sar_den = den * fmt.video.i_width;
            }
        }
        if (!var_InheritURational(object, &num, &den, "imem-fps") && num > 0 && den > 0) {
            fmt.video.i_frame_rate      = num;
            fmt.video.i_frame_rate_base = den;
        }
        msg_Dbg(object, "Video %4.4s %dx%d  SAR %d:%d frame rate %u/%u",
                (const char *)&fmt.i_codec,
                fmt.video.i_width, fmt.video.i_height,
                fmt.video.i_sar_num, fmt.video.i_sar_den,
                fmt.video.i_frame_rate, fmt.video.i_frame_rate_base);
        break;
    }
    case 3: {
        fmt.i_cat = SPU_ES;
        fmt.subs.spu.i_original_frame_width =
            var_InheritInteger(object, "imem-width");
        fmt.subs.spu.i_original_frame_height =
            var_InheritInteger(object, "imem-height");
        msg_Dbg(object, "Subtitle %4.4s",
                (const char *)&fmt.i_codec);
        break;
    }
    default:
        if (cat != 4)
            msg_Err(object, "Invalid ES category");
        es_format_Clean(&fmt);
        CloseCommon(sys);
        return VLC_EGENERIC;
    }
    fmt.psz_language = var_InheritString(object, "imem-language");
    sys->es = es_out_Add(demux->out, &fmt);
    es_format_Clean(&fmt);
    if (!sys->es) {
        CloseCommon(sys);
        return VLC_EGENERIC;
    }
    /* */
    demux->pf_control = ControlDemux;
    demux->pf_demux   = Demux;
    demux->p_sys      = (demux_sys_t*)sys;
    demux->info.i_update = 0;
    demux->info.i_title = 0;
    demux->info.i_seekpoint = 0;
    return VLC_SUCCESS;
}
