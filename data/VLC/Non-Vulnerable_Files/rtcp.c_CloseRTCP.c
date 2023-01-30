}
void CloseRTCP (rtcp_sender_t *rtcp)
{
    if (rtcp == NULL)
        return;
    uint8_t *ptr = rtcp->payload;
    uint64_t now64 = NTPtime64 ();
    SetQWBE (ptr + 8, now64); /* Update the Sender Report timestamp */
    /* Bye */
    ptr += rtcp->length;
    ptr[0] = (2 << 6) | 1; /* V = 2, P = 0, SC = 1 */
    ptr[1] = 203; /* payload type: Bye */
    SetWBE (ptr + 2, 1);
    memcpy (ptr + 4, rtcp->payload + 4, 4); /* Copy SSRC from Sender Report */
    rtcp->length += 8;
    /* We are THE sender, so we are more important than anybody else, so
     * we can afford not to check bandwidth constraints here. */
    send (rtcp->handle, rtcp->payload, rtcp->length, 0);
    net_Close (rtcp->handle);
    free (rtcp);
}
