static av_always_inline void simple_rotate(uint8_t *dst, const uint8_t *src, int src_linesize, int angle, int elem_size, int len)
{
    switch(elem_size) {
    case 1 : simple_rotate_internal(dst, src, src_linesize, angle, 1, len); break;
    case 2 : simple_rotate_internal(dst, src, src_linesize, angle, 2, len); break;
    case 3 : simple_rotate_internal(dst, src, src_linesize, angle, 3, len); break;
    case 4 : simple_rotate_internal(dst, src, src_linesize, angle, 4, len); break;
    default: simple_rotate_internal(dst, src, src_linesize, angle, elem_size, len); break;
    }
}
