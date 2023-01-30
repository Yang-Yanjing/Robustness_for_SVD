static void
rtp_decode (demux_t *demux, const rtp_session_t *session, rtp_source_t *src)
{
    block_t *block = src->blocks;
    assert (block);
    src->blocks = block->p_next;
    block->p_next = NULL;
    /* Discontinuity detection */
    uint16_t delta_seq = rtp_seq (block) - (src->last_seq + 1);
    if (delta_seq != 0)
    {
        if (delta_seq >= 0x8000)
        {   /* Trash too late packets (and PIM Assert duplicates) */
            msg_Dbg (demux, "ignoring late packet (sequence: %"PRIu16")",
                      rtp_seq (block));
            goto drop;
        }
        msg_Warn (demux, "%"PRIu16" packet(s) lost", delta_seq);
        block->i_flags |= BLOCK_FLAG_DISCONTINUITY;
    }
    src->last_seq = rtp_seq (block);
    /* Match the payload type */
    void *pt_data;
    const rtp_pt_t *pt = rtp_find_ptype (session, src, block, &pt_data);
    if (pt == NULL)
    {
        msg_Dbg (demux, "unknown payload (%"PRIu8")",
                 rtp_ptype (block));
        goto drop;
    }
    /* Computes the PTS from the RTP timestamp and payload RTP frequency.
     * DTS is unknown. Also, while the clock frequency depends on the payload
     * format, a single source MUST only use payloads of a chosen frequency.
     * Otherwise it would be impossible to compute consistent timestamps. */
    const uint32_t timestamp = rtp_timestamp (block);
    block->i_pts = src->ref_ntp
       + CLOCK_FREQ * (int32_t)(timestamp - src->ref_rtp) / pt->frequency;
    /* TODO: proper inter-medias/sessions sync (using RTCP-SR) */
    src->ref_ntp = block->i_pts;
    src->ref_rtp = timestamp;
    /* CSRC count */
    size_t skip = 12u + (block->p_buffer[0] & 0x0F) * 4;
    /* Extension header (ignored for now) */
    if (block->p_buffer[0] & 0x10)
    {
        skip += 4;
        if (block->i_buffer < skip)
            goto drop;
        skip += 4 * GetWBE (block->p_buffer + skip - 2);
    }
    if (block->i_buffer < skip)
        goto drop;
    block->p_buffer += skip;
    block->i_buffer -= skip;
    pt->decode (demux, pt_data, block);
    return;
drop:
    block_Release (block);
}
