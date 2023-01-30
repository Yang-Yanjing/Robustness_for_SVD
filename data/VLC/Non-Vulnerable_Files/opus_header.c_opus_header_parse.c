}
int opus_header_parse(const unsigned char *packet, int len, OpusHeader *h)
{
    char str[9];
    ROPacket p;
    unsigned char ch;
    uint16_t shortval;
    p.data = packet;
    p.maxlen = len;
    p.pos = 0;
    str[8] = 0;
    if (len<19)return 0;
    read_chars(&p, (unsigned char*)str, 8);
    if (memcmp(str, "OpusHead", 8)!=0)
        return 0;
    if (!read_chars(&p, &ch, 1))
        return 0;
    h->version = ch;
    if((h->version&240) != 0) /* Only major version 0 supported. */
        return 0;
    if (!read_chars(&p, &ch, 1))
        return 0;
    h->channels = ch;
    if (h->channels == 0)
        return 0;
    if (!read_uint16(&p, &shortval))
        return 0;
    h->preskip = shortval;
    if (!read_uint32(&p, &h->input_sample_rate))
        return 0;
    if (!read_uint16(&p, &shortval))
        return 0;
    h->gain = (short)shortval;
    if (!read_chars(&p, &ch, 1))
        return 0;
    h->channel_mapping = ch;
    if (h->channel_mapping != 0)
    {
        if (!read_chars(&p, &ch, 1))
            return 0;
        if (ch<1)
            return 0;
        h->nb_streams = ch;
        if (!read_chars(&p, &ch, 1))
            return 0;
        if (ch>h->nb_streams || (ch+h->nb_streams)>255)
            return 0;
        h->nb_coupled = ch;
        /* Multi-stream support */
        for (int i=0;i<h->channels;i++)
        {
            if (!read_chars(&p, &h->stream_map[i], 1))
                return 0;
            if (h->stream_map[i]>(h->nb_streams+h->nb_coupled) && h->stream_map[i]!=255)
                return 0;
        }
    } else {
        if(h->channels>2)
            return 0;
        h->nb_streams = 1;
        h->nb_coupled = h->channels>1;
        h->stream_map[0]=0;
        h->stream_map[1]=1;
    }
    /*For version 0/1 we know there won't be any more data
      so reject any that have data past the end.*/
    if ((h->version==0 || h->version==1) && p.pos != len)
        return 0;
    return 1;
}
