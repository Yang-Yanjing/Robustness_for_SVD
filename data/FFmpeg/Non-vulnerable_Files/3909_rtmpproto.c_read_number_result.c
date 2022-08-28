static int read_number_result(RTMPPacket *pkt, double *number)
{
    
    uint8_t strbuffer[8];
    int stringlen;
    double numbuffer;
    GetByteContext gbc;
    bytestream2_init(&gbc, pkt->data, pkt->size);
    
    if (ff_amf_read_string(&gbc, strbuffer, sizeof(strbuffer), &stringlen))
        return AVERROR_INVALIDDATA;
    if (strcmp(strbuffer, "_result"))
        return AVERROR_INVALIDDATA;
    
    if (ff_amf_read_number(&gbc, &numbuffer))
        return AVERROR_INVALIDDATA;
    
    if (ff_amf_read_null(&gbc))
        return AVERROR_INVALIDDATA;
    
    if (ff_amf_read_number(&gbc, &numbuffer))
        return AVERROR_INVALIDDATA;
    else
        *number = numbuffer;
    return 0;
}
