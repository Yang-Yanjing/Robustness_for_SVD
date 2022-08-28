static uint8_t *nal_unit_extract_rbsp(const uint8_t *src, uint32_t src_len,
                                      uint32_t *dst_len)
{
    uint8_t *dst;
    uint32_t i, len;
    dst = av_malloc(src_len + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!dst)
        return NULL;
    
    i = len = 0;
    while (i < 2 && i < src_len)
        dst[len++] = src[i++];
    while (i + 2 < src_len)
        if (!src[i] && !src[i + 1] && src[i + 2] == 3) {
            dst[len++] = src[i++];
            dst[len++] = src[i++];
            i++; 
        } else
            dst[len++] = src[i++];
    while (i < src_len)
        dst[len++] = src[i++];
    *dst_len = len;
    return dst;
}
