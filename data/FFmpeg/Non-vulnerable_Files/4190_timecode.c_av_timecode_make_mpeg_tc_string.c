char *av_timecode_make_mpeg_tc_string(char *buf, uint32_t tc25bit)
{
    snprintf(buf, AV_TIMECODE_STR_SIZE, "%02u:%02u:%02u%c%02u",
             tc25bit>>19 & 0x1f,              
             tc25bit>>13 & 0x3f,              
             tc25bit>>6  & 0x3f,              
             tc25bit     & 1<<24 ? ';' : ':', 
             tc25bit     & 0x3f);             
    return buf;
}
