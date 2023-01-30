}
static int opus_header_to_packet(const OpusHeader *h, unsigned char *packet, int len)
{
    Packet p;
    unsigned char ch;
    p.data = packet;
    p.maxlen = len;
    p.pos = 0;
    if (len<19)return 0;
    if (!write_chars(&p, (const unsigned char*)"OpusHead", 8))
        return 0;
    /* Version is 1 */
    ch = 1;
    if (!write_chars(&p, &ch, 1))
        return 0;
    ch = h->channels;
    if (!write_chars(&p, &ch, 1))
        return 0;
    if (!write_uint16(&p, h->preskip))
        return 0;
    if (!write_uint32(&p, h->input_sample_rate))
        return 0;
    if (!write_uint16(&p, h->gain))
        return 0;
    ch = h->channel_mapping;
    if (!write_chars(&p, &ch, 1))
        return 0;
    if (h->channel_mapping != 0)
    {
        ch = h->nb_streams;
        if (!write_chars(&p, &ch, 1))
            return 0;
        ch = h->nb_coupled;
        if (!write_chars(&p, &ch, 1))
            return 0;
        /* Multi-stream support */
        for (int i=0;i<h->channels;i++)
        {
            if (!write_chars(&p, &h->stream_map[i], 1))
                return 0;
        }
    }
    return p.pos;
}
