static void lowpass_line_c(uint8_t *dstp, ptrdiff_t width, const uint8_t *srcp,
                           const uint8_t *srcp_above, const uint8_t *srcp_below)
{
    int i;
    for (i = 0; i < width; i++) {
        
        
        
        dstp[i] = (1 + srcp[i] + srcp[i] + srcp_above[i] + srcp_below[i]) >> 2;
    }
}
