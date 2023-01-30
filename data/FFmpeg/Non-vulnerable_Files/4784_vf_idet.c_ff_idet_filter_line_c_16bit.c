int ff_idet_filter_line_c_16bit(const uint16_t *a, const uint16_t *b, const uint16_t *c, int w)
{
    int x;
    int ret=0;
    for(x=0; x<w; x++){
        int v = (*a++ + *c++) - 2 * *b++;
        ret += FFABS(v);
    }
    return ret;
}
