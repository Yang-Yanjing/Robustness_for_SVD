static av_always_inline void simple_rotate_internal(uint8_t *dst, const uint8_t *src, int src_linesize, int angle, int elem_size, int len)
{
    int i;
    switch(angle) {
    case 0:
        memcpy(dst, src, elem_size * len);
        break;
    case 1:
        for (i = 0; i<len; i++)
            copy_elem(dst + i*elem_size, src + (len-i-1)*src_linesize, elem_size);
        break;
    case 2:
        for (i = 0; i<len; i++)
            copy_elem(dst + i*elem_size, src + (len-i-1)*elem_size, elem_size);
        break;
    case 3:
        for (i = 0; i<len; i++)
            copy_elem(dst + i*elem_size, src + i*src_linesize, elem_size);
        break;
    }
}
