static int handle_notify(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt  = s->priv_data;
    uint8_t commandbuffer[64];
    char statusmsg[128];
    int stringlen, ret, skip = 0;
    GetByteContext gbc;
    bytestream2_init(&gbc, pkt->data, pkt->size);
    if (ff_amf_read_string(&gbc, commandbuffer, sizeof(commandbuffer),
                           &stringlen))
        return AVERROR_INVALIDDATA;
    if (!strcmp(commandbuffer, "onMetaData")) {
        
        if (bytestream2_get_byte(&gbc) == AMF_DATA_TYPE_MIXEDARRAY) {
            
            
            rt->received_metadata = 1;
            
            bytestream2_skip(&gbc, 4);
            while (bytestream2_get_bytes_left(&gbc) > 3) {
                if (ff_amf_get_string(&gbc, statusmsg, sizeof(statusmsg),
                                      &stringlen))
                    return AVERROR_INVALIDDATA;
                
                stringlen = ff_amf_tag_size(gbc.buffer, gbc.buffer_end);
                if (stringlen < 0)
                    return AVERROR_INVALIDDATA;
                bytestream2_skip(&gbc, stringlen);
                
                
                if (!strcmp(statusmsg, "videocodecid")) {
                    rt->has_video = 1;
                }
                if (!strcmp(statusmsg, "audiocodecid")) {
                    rt->has_audio = 1;
                }
            }
            if (bytestream2_get_be24(&gbc) != AMF_END_OF_OBJECT)
                return AVERROR_INVALIDDATA;
        }
    }
    
    if (!strcmp(commandbuffer, "@setDataFrame")) {
        skip = gbc.buffer - pkt->data;
        ret = ff_amf_read_string(&gbc, statusmsg,
                                 sizeof(statusmsg), &stringlen);
        if (ret < 0)
            return AVERROR_INVALIDDATA;
    }
    return append_flv_data(rt, pkt, skip);
}
