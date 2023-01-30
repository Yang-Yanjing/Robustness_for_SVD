}
char *sdp_AddMedia (char **sdp,
                    const char *type, const char *protocol, int dport,
                    unsigned pt, bool bw_indep, unsigned bw,
                    const char *ptname, unsigned clock, unsigned chans,
                    const char *fmtp)
{
    char *newsdp, *ptr;
    size_t inlen = strlen (*sdp), outlen = inlen;
    /* Some default values */
    if (type == NULL)
        type = "video";
    if (protocol == NULL)
        protocol = "RTP/AVP";
    assert (pt < 128u);
    outlen += snprintf (NULL, 0,
                        "m=%s %u %s %d\r\n"
                        "b=TIAS:%u\r\n"
                        "b=RR:0\r\n",
                        type, dport, protocol, pt, bw);
    newsdp = realloc (*sdp, outlen + 1);
    if (newsdp == NULL)
        return NULL;
    *sdp = newsdp;
    ptr = newsdp + inlen;
    ptr += sprintf (ptr, "m=%s %u %s %u\r\n",
                         type, dport, protocol, pt);
    if (bw > 0)
        ptr += sprintf (ptr, "b=%s:%u\r\n", bw_indep ? "TIAS" : "AS", bw);
    ptr += sprintf (ptr, "b=RR:0\r\n");
    /* RTP payload type map */
    if (ptname != NULL)
    {
        if ((strcmp (type, "audio") == 0) && (chans != 1))
            sdp_AddAttribute (sdp, "rtpmap", "%u %s/%u/%u", pt, ptname, clock,
                              chans);
        else
            sdp_AddAttribute (sdp, "rtpmap", "%u %s/%u", pt, ptname, clock);
    }
    /* Format parameters */
    if (fmtp != NULL)
        sdp_AddAttribute (sdp, "fmtp", "%u %s", pt, fmtp);
    return newsdp;
}
