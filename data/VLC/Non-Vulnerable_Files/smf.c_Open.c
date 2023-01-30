 */
static int Open (vlc_object_t *obj)
{
    demux_t *demux = (demux_t *)obj;
    stream_t *stream = demux->s;
    const uint8_t *peek;
    bool multitrack;
    /* (Try to) parse the SMF header */
    /* Header chunk always has 6 bytes payload */
    if (stream_Peek (stream, &peek, 14) < 14)
        return VLC_EGENERIC;
    /* Skip RIFF MIDI header if present */
    if (!memcmp (peek, "RIFF", 4) && !memcmp (peek + 8, "RMID", 4))
    {
        uint32_t riff_len = GetDWLE (peek + 4);
        msg_Dbg (demux, "detected RIFF MIDI file (%"PRIu32" bytes)", riff_len);
        if ((stream_Read (stream, NULL, 12) < 12))
            return VLC_EGENERIC;
        /* Look for the RIFF data chunk */
        for (;;)
        {
            char chnk_hdr[8];
            uint32_t chnk_len;
            if ((riff_len < 8)
             || (stream_Read (stream, chnk_hdr, 8) < 8))
                return VLC_EGENERIC;
            riff_len -= 8;
            chnk_len = GetDWLE (chnk_hdr + 4);
            if (riff_len < chnk_len)
                return VLC_EGENERIC;
            riff_len -= chnk_len;
            if (!memcmp (chnk_hdr, "data", 4))
                break; /* found! */
            if (stream_Read (stream, NULL, chnk_len) < (ssize_t)chnk_len)
                return VLC_EGENERIC;
        }
        /* Read real SMF header. Assume RIFF data chunk length is proper. */
        if (stream_Peek (stream, &peek, 14) < 14)
            return VLC_EGENERIC;
    }
    if (memcmp (peek, "MThd\x00\x00\x00\x06", 8))
        return VLC_EGENERIC;
    peek += 8;
    /* First word: SMF type */
    switch (GetWBE (peek))
    {
        case 0:
            multitrack = false;
            break;
        case 1:
            multitrack = true;
            break;
        default:
            /* We don't implement SMF2 (as do many) */
            msg_Err (demux, "unsupported SMF file type %u", GetWBE (peek));
            return VLC_EGENERIC;
    }
    peek += 2;
    /* Second word: number of tracks */
    unsigned tracks = GetWBE (peek);
    peek += 2;
    if (!multitrack && (tracks != 1))
    {
        msg_Err (demux, "invalid SMF type 0 file");
        return VLC_EGENERIC;
    }
    msg_Dbg (demux, "detected Standard MIDI File (type %u) with %u track(s)",
             multitrack, tracks);
    /* Third/last word: timing */
    unsigned ppqn = GetWBE (peek);
    if (ppqn & 0x8000)
    {   /* FIXME */
        msg_Err (demux, "SMPTE timestamps not implemented");
        return VLC_EGENERIC;
    }
    else
    {
        if (ppqn == 0)
        {
            msg_Err(demux, "invalid SMF file PPQN: %u", ppqn);
            return VLC_EGENERIC;
        }
        msg_Dbg (demux, " %u pulses per quarter note", ppqn);
    }
    demux_sys_t *sys = malloc (sizeof (*sys) + (sizeof (mtrk_t) * tracks));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    /* We've had a valid SMF header - now skip it*/
    if (stream_Read (stream, NULL, 14) < 14)
        goto error;
    demux->p_sys = sys;
    sys->duration = 0;
    sys->ppqn = ppqn;
    sys->trackc = tracks;
    /* Prefetch track offsets */
    for (unsigned i = 0; i < tracks; i++)
    {
        mtrk_t *tr = sys->trackv + i;
        uint8_t head[8];
        /* Seeking screws streaming up, but there is no way around this, as
         * SMF1 tracks are performed simultaneously.
         * Not a big deal as SMF1 are usually only a few kbytes anyway. */
        if (i > 0 && stream_Seek (stream, tr[-1].start + tr[-1].length))
        {
            msg_Err (demux, "cannot build SMF index (corrupted file?)");
            goto error;
        }
        for (;;)
        {
            if (stream_Read (stream, head, 8) < 8)
            {
                /* FIXME: don't give up if we have at least one valid track */
                msg_Err (demux, "incomplete SMF chunk, file is corrupted");
                goto error;
            }
            if (memcmp (head, "MTrk", 4) == 0)
                break;
            msg_Dbg (demux, "skipping unknown SMF chunk");
            stream_Read (stream, NULL, GetDWBE (head + 4));
        }
        tr->start = stream_Tell (stream);
        tr->length = GetDWBE (head + 4);
    }
    bool b;
    if (stream_Control (stream, STREAM_CAN_FASTSEEK, &b) == 0 && b)
    {
        if (SeekSet0 (demux))
            goto error;
        for (uint64_t pulse = 0; pulse != UINT64_MAX;)
             if (ReadEvents (demux, &pulse, NULL))
                 break;
        sys->duration = date_Get (&sys->pts);
    }
    if (SeekSet0 (demux))
        goto error;
    es_format_t  fmt;
    es_format_Init (&fmt, AUDIO_ES, VLC_CODEC_MIDI);
    fmt.audio.i_channels = 2;
    fmt.audio.i_rate = 44100; /* dummy value */
    sys->es = es_out_Add (demux->out, &fmt);
    demux->pf_demux = Demux;
    demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    free (sys);
    return VLC_EGENERIC;
}
