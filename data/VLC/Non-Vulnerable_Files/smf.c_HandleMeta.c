static
int HandleMeta (demux_t *p_demux, mtrk_t *tr)
{
    stream_t *s = p_demux->s;
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t *payload;
    uint8_t type;
    int32_t length;
    int ret = 0;
    if (stream_Read (s, &type, 1) != 1)
        return -1;
    length = ReadVarInt (s);
    if (length < 0)
        return -1;
    payload = malloc (length + 1);
    if ((payload == NULL)
     || (stream_Read (s, payload, length) != length))
    {
        free (payload);
        return -1;
    }
    payload[length] = '\0';
    switch (type)
    {
        case 0x00: /* Sequence Number */
            break;
        case 0x01: /* Text (comment) */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Text      : %s", (char *)payload);
            break;
        case 0x02: /* Copyright */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Copyright : %s", (char *)payload);
            break;
        case 0x03: /* Track name */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Track name: %s", (char *)payload);
            break;
        case 0x04: /* Instrument name */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Instrument: %s", (char *)payload);
            break;
        case 0x05: /* Lyric (one syllable) */
            /*EnsureUTF8 ((char *)payload);*/
            break;
        case 0x06: /* Marker text */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Marker    : %s", (char *)payload);
        case 0x07: /* Cue point (WAVE filename) */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Cue point : %s", (char *)payload);
            break;
        case 0x08: /* Program/Patch name */
            EnsureUTF8 ((char *)payload);
            msg_Info (p_demux, "Patch name: %s", (char *)payload);
            break;
        case 0x09: /* MIDI port name */
            EnsureUTF8 ((char *)payload);
            msg_Dbg (p_demux, "MIDI port : %s", (char *)payload);
            break;
        case 0x2F: /* End of track */
            if (tr->start + tr->length != stream_Tell (s))
            {
                msg_Err (p_demux, "misplaced end of track");
                ret = -1;
            }
            break;
        case 0x51: /* Tempo */
            if (length == 3)
            {
                uint32_t uspqn = (payload[0] << 16)
                               | (payload[1] << 8) | payload[2];
                unsigned tempo = 60 * 1000000 / (uspqn ? uspqn : 1);
                msg_Dbg (p_demux, "tempo: %uus/qn -> %u BPM",
                         (unsigned)uspqn, tempo);
                if (tempo < TEMPO_MIN)
                {
                    msg_Warn (p_demux, "tempo too slow -> %u BPM", TEMPO_MIN);
                    tempo = TEMPO_MIN;
                }
                else
                if (tempo > TEMPO_MAX)
                {
                    msg_Warn (p_demux, "tempo too fast -> %u BPM", TEMPO_MAX);
                    tempo = TEMPO_MAX;
                }
                date_Change (&p_sys->pts, p_sys->ppqn * tempo, 60);
            }
            else
                ret = -1;
            break;
        case 0x54: /* SMPTE offset */
            if (length == 5)
                msg_Warn (p_demux, "SMPTE offset not implemented");
            else
                ret = -1;
            break;
        case 0x58: /* Time signature */
            if (length == 4)
                ;
            else
                ret = -1;
            break;
        case 0x59: /* Key signature */
            if (length == 2)
                ;
            else
                ret = -1;
            break;
        case 0x7f: /* Proprietary event */
            msg_Dbg (p_demux, "ignored proprietary SMF Meta Event (%d bytes)",
                     length);
            break;
        default:
            msg_Warn (p_demux, "unknown SMF Meta Event type 0x%02X (%d bytes)",
                      type, length);
    }
    free (payload);
    return ret;
}
