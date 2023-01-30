}
void SendRTCP (rtcp_sender_t *restrict rtcp, const block_t *rtp)
{
    if ((rtcp == NULL) /* RTCP sender off */
     || (rtp->i_buffer < 12)) /* too short RTP packet */
        return;
    /* Updates statistics */
    rtcp->packets++;
    rtcp->bytes += rtp->i_buffer;
    rtcp->counter += rtp->i_buffer;
    /* 1.25% rate limit */
    if ((rtcp->counter / 80) < rtcp->length)
        return;
    uint8_t *ptr = rtcp->payload;
    uint32_t last = GetDWBE (ptr + 8); // last RTCP SR send time
    uint64_t now64 = NTPtime64 ();
    if ((now64 >> 32) < (last + 5))
        return; // no more than one SR every 5 seconds
    memcpy (ptr + 4, rtp->p_buffer + 8, 4); /* SR SSRC */
    SetQWBE (ptr + 8, now64);
    memcpy (ptr + 16, rtp->p_buffer + 4, 4); /* RTP timestamp */
    SetDWBE (ptr + 20, rtcp->packets);
    SetDWBE (ptr + 24, rtcp->bytes);
    memcpy (ptr + 28 + 4, rtp->p_buffer + 8, 4); /* SDES SSRC */
    if (send (rtcp->handle, ptr, rtcp->length, 0) == (ssize_t)rtcp->length)
        rtcp->counter = 0;
}
