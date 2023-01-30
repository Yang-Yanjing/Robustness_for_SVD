AVFILTER_DEFINE_CLASS(interlace);
static void lowpass_line_c(uint8_t *dstp, ptrdiff_t linesize,
                           const uint8_t *srcp,
                           const uint8_t *srcp_above,
                           const uint8_t *srcp_below)
{
    int i;
    for (i = 0; i < linesize; i++) {
        
        
        
        dstp[i] = (1 + srcp[i] + srcp[i] + srcp_above[i] + srcp_below[i]) >> 2;
    }
}
