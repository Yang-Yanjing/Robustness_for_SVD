 */
bool rtp_dequeue (demux_t *demux, const rtp_session_t *session,
                  mtime_t *restrict deadlinep)
{
    mtime_t now = mdate ();
    bool pending = false;
    *deadlinep = INT64_MAX;
    for (unsigned i = 0, max = session->srcc; i < max; i++)
    {
        rtp_source_t *src = session->srcv[i];
        block_t *block;
        /* Because of IP packet delay variation (IPDV), we need to guesstimate
         * how long to wait for a missing packet in the RTP sequence
         * (see RFC3393 for background on IPDV).
         *
         * This situation occurs if a packet got lost, or if the network has
         * re-ordered packets. Unfortunately, the MSL is 2 minutes, orders of
         * magnitude too long for multimedia. We need a trade-off.
         * If we underestimated IPDV, we may have to discard valid but late
         * packets. If we overestimate it, we will either cause too much
         * delay, or worse, underflow our downstream buffers, as we wait for
         * definitely a lost packets.
         *
         * The rest of the "de-jitter buffer" work is done by the internal
         * LibVLC E/S-out clock synchronization. Here, we need to bother about
         * re-ordering packets, as decoders can't cope with mis-ordered data.
         */
        while (((block = src->blocks)) != NULL)
        {
            if ((int16_t)(rtp_seq (block) - (src->last_seq + 1)) <= 0)
            {   /* Next (or earlier) block ready, no need to wait */
                rtp_decode (demux, session, src);
                continue;
            }
            /* Wait for 3 times the inter-arrival delay variance (about 99.7%
             * match for random gaussian jitter).
             */
            mtime_t deadline;
            const rtp_pt_t *pt = rtp_find_ptype (session, src, block, NULL);
            if (pt)
                deadline = CLOCK_FREQ * 3 * src->jitter / pt->frequency;
            else
                deadline = 0; /* no jitter estimate with no frequency :( */
            /* Make sure we wait at least for 25 msec */
            if (deadline < (CLOCK_FREQ / 40))
                deadline = CLOCK_FREQ / 40;
            /* Additionnaly, we implicitly wait for the packetization time
             * multiplied by the number of missing packets. block is the first
             * non-missing packet (lowest sequence number). We have no better
             * estimated time of arrival, as we do not know the RTP timestamp
             * of not yet received packets. */
            deadline += block->i_pts;
            if (now >= deadline)
            {
                rtp_decode (demux, session, src);
                continue;
            }
            if (*deadlinep > deadline)
                *deadlinep = deadline;
            pending = true; /* packet pending in buffer */
            break;
        }
    }
    return pending;
}
