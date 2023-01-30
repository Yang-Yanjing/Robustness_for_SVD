/** Detect dvb-utils zap channels.conf format */
int Import_DVB(vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    if (!demux_IsPathExtension(demux, ".conf" ) && !demux->b_force )
        return VLC_EGENERIC;
    /* Check if this really is a channels file */
    const uint8_t *peek;
    int len = stream_Peek(demux->s, &peek, 1023);
    if (len <= 0)
        return VLC_EGENERIC;
    const uint8_t *eol = memchr(peek, '\n', len);
    if (eol == NULL)
        return VLC_EGENERIC;
    len = eol - peek;
    char line[len + 1];
    memcpy(line, peek, len);
    line[len] = '\0';
    input_item_t *item = ParseLine(line);
    if (item == NULL)
        return VLC_EGENERIC;
    vlc_gc_decref(item);
    msg_Dbg(demux, "found valid channels.conf file");
    demux->pf_control = Control;
    demux->pf_demux = Demux;
    return VLC_SUCCESS;
}
