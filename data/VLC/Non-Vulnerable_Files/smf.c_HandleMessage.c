static
int HandleMessage (demux_t *p_demux, mtrk_t *tr, es_out_t *out)
{
    stream_t *s = p_demux->s;
    block_t *block;
    uint8_t first, event;
    unsigned datalen;
    if (stream_Seek (s, tr->start + tr->offset)
     || (stream_Read (s, &first, 1) != 1))
        return -1;
    event = (first & 0x80) ? first : tr->running_event;
    switch (event & 0xf0)
    {
        case 0xF0: /* System Exclusive */
            switch (event)
            {
                case 0xF0: /* System Specific start */
                case 0xF7: /* System Specific continuation */
                {
                    /* Variable length followed by SysEx event data */
                    int32_t len = ReadVarInt (s);
                    if (len == -1)
                        return -1;
                    block = stream_Block (s, len);
                    if (block == NULL)
                        return -1;
                    block = block_Realloc (block, 1, len);
                    if (block == NULL)
                        return -1;
                    block->p_buffer[0] = event;
                    goto send;
                }
                case 0xFF: /* SMF Meta Event */
                    if (HandleMeta (p_demux, tr))
                        return -1;
                    /* We MUST NOT pass this event forward. It would be
                     * confused as a MIDI Reset real-time event. */
                    goto skip;
                case 0xF1:
                case 0xF3:
                    datalen = 1;
                    break;
                case 0xF2:
                    datalen = 2;
                    break;
                case 0xF4:
                case 0xF5:
                    /* We cannot handle undefined "common" (non-real-time)
                     * events inside SMF, as we cannot differentiate a
                     * one byte delta-time (< 0x80) from event data. */
                default:
                    datalen = 0;
                    break;
            }
            break;
        case 0xC0:
        case 0xD0:
            datalen = 1;
            break;
        default:
            datalen = 2;
            break;
    }
    /* FIXME: one message per block is very inefficient */
    block = block_Alloc (1 + datalen);
    if (block == NULL)
        goto skip;
    block->p_buffer[0] = event;
    if (first & 0x80)
    {
        stream_Read (s, block->p_buffer + 1, datalen);
    }
    else
    {
        if (datalen == 0)
        {
            msg_Err (p_demux, "malformatted MIDI event");
            block_Release(block);
            return -1; /* implicit running status requires non-empty payload */
        }
        block->p_buffer[1] = first;
        if (datalen > 1)
            stream_Read (s, block->p_buffer + 2, datalen - 1);
    }
send:
    block->i_dts = block->i_pts = date_Get (&p_demux->p_sys->pts);
    if (out != NULL)
        es_out_Send (out, p_demux->p_sys->es, block);
    else
        block_Release (block);
skip:
    if (event < 0xF8)
        /* If event is not real-time, update running status */
        tr->running_event = event;
    tr->offset = stream_Tell (s) - tr->start;
    return 0;
}
