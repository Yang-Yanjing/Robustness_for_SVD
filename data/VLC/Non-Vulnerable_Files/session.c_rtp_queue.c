void
rtp_queue (demux_t *demux, rtp_session_t *session, block_t *block)
{
    demux_sys_t *p_sys = demux->p_sys;
    /* RTP header sanity checks (see RFC 3550) */
    if (block->i_buffer < 12)
        goto drop;
    if ((block->p_buffer[0] >> 6 ) != 2) /* RTP version number */
        goto drop;
    /* Remove padding if present */
    if (block->p_buffer[0] & 0x20)
    {
        uint8_t padding = block->p_buffer[block->i_buffer - 1];
        if ((padding == 0) || (block->i_buffer < (12u + padding)))
            goto drop; /* illegal value */
        block->i_buffer -= padding;
    }
    mtime_t        now = mdate ();
    rtp_source_t  *src  = NULL;
    const uint16_t seq  = rtp_seq (block);
    const uint32_t ssrc = GetDWBE (block->p_buffer + 8);
    /* In most case, we know this source already */
    for (unsigned i = 0, max = session->srcc; i < max; i++)
    {
        rtp_source_t *tmp = session->srcv[i];
        if (tmp->ssrc == ssrc)
        {
            src = tmp;
            break;
        }
        /* RTP source garbage collection */
        if ((tmp->last_rx + p_sys->timeout) < now)
        {
            rtp_source_destroy (demux, session, tmp);
            if (--session->srcc > 0)
                session->srcv[i] = session->srcv[session->srcc - 1];
        }
    }
    if (src == NULL)
    {
        /* New source */
        if (session->srcc >= p_sys->max_src)
        {
            msg_Warn (demux, "too many RTP sessions");
            goto drop;
        }
        rtp_source_t **tab;
        tab = realloc (session->srcv, (session->srcc + 1) * sizeof (*tab));
        if (tab == NULL)
            goto drop;
        session->srcv = tab;
        src = rtp_source_create (demux, session, ssrc, seq);
        if (src == NULL)
            goto drop;
        tab[session->srcc++] = src;
        /* Cannot compute jitter yet */
    }
    else
    {
        const rtp_pt_t *pt = rtp_find_ptype (session, src, block, NULL);
        if (pt != NULL)
        {
            /* Recompute jitter estimate.
             * That is computed from the RTP timestamps and the system clock.
             * It is independent of RTP sequence. */
            uint32_t freq = pt->frequency;
            int64_t ts = rtp_timestamp (block);
            int64_t d = ((now - src->last_rx) * freq) / CLOCK_FREQ;
            d        -=    ts - src->last_ts;
            if (d < 0) d = -d;
            src->jitter += ((d - src->jitter) + 8) >> 4;
        }
    }
    src->last_rx = now;
    block->i_pts = now; /* store reception time until dequeued */
    src->last_ts = rtp_timestamp (block);
    /* Check sequence number */
    /* NOTE: the sequence number is per-source,
     * but is independent from the payload type. */
    int16_t delta_seq = seq - src->max_seq;
    if ((delta_seq > 0) ? (delta_seq > p_sys->max_dropout)
                        : (-delta_seq > p_sys->max_misorder))
    {
        msg_Dbg (demux, "sequence discontinuity"
                 " (got: %"PRIu16", expected: %"PRIu16")", seq, src->max_seq);
        if (seq == src->bad_seq)
        {
            src->max_seq = src->bad_seq = seq + 1;
            src->last_seq = seq - 0x7fffe; /* hack for rtp_decode() */
            msg_Warn (demux, "sequence resynchronized");
            block_ChainRelease (src->blocks);
            src->blocks = NULL;
        }
        else
        {
            src->bad_seq = seq + 1;
            goto drop;
        }
    }
    else
    if (delta_seq >= 0)
        src->max_seq = seq + 1;
    /* Queues the block in sequence order,
     * hence there is a single queue for all payload types. */
    block_t **pp = &src->blocks;
    for (block_t *prev = *pp; prev != NULL; prev = *pp)
    {
        int16_t delta_seq = seq - rtp_seq (prev);
        if (delta_seq < 0)
            break;
        if (delta_seq == 0)
        {
            msg_Dbg (demux, "duplicate packet (sequence: %"PRIu16")", seq);
            goto drop; /* duplicate */
        }
        pp = &prev->p_next;
    }
    block->p_next = *pp;
    *pp = block;
    /*rtp_decode (demux, session, src);*/
    return;
drop:
    block_Release (block);
}
