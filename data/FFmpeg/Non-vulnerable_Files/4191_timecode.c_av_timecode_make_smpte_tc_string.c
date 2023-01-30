char *av_timecode_make_smpte_tc_string(char *buf, uint32_t tcsmpte, int prevent_df)
{
    unsigned hh   = bcd2uint(tcsmpte     & 0x3f);    
    unsigned mm   = bcd2uint(tcsmpte>>8  & 0x7f);    
    unsigned ss   = bcd2uint(tcsmpte>>16 & 0x7f);    
    unsigned ff   = bcd2uint(tcsmpte>>24 & 0x3f);    
    unsigned drop = tcsmpte & 1<<30 && !prevent_df;  
    snprintf(buf, AV_TIMECODE_STR_SIZE, "%02u:%02u:%02u%c%02u",
             hh, mm, ss, drop ? ';' : ':', ff);
    return buf;
}
