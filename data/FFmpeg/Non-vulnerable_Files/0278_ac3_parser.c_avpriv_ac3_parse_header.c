int avpriv_ac3_parse_header(GetBitContext *gbc, AC3HeaderInfo *hdr)
{
    AC3HeaderInfo tmp, *ptmp = &tmp;
    int ret = avpriv_ac3_parse_header2(gbc, &ptmp);
    memcpy(hdr, ptmp, ((intptr_t)&tmp.channel_layout) - ((intptr_t)&tmp) + sizeof(uint64_t));
    return ret;
}
