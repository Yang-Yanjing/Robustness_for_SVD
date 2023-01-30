static int matroska_parse_wavpack(MatroskaTrack *track, uint8_t *src,
                                  uint8_t **pdst, int *size)
{
    uint8_t *dst = NULL;
    int dstlen   = 0;
    int srclen   = *size;
    uint32_t samples;
    uint16_t ver;
    int ret, offset = 0;
    if (srclen < 12 || track->stream->codec->extradata_size < 2)
        return AVERROR_INVALIDDATA;
    ver = AV_RL16(track->stream->codec->extradata);
    samples = AV_RL32(src);
    src    += 4;
    srclen -= 4;
    while (srclen >= 8) {
        int multiblock;
        uint32_t blocksize;
        uint8_t *tmp;
        uint32_t flags = AV_RL32(src);
        uint32_t crc   = AV_RL32(src + 4);
        src    += 8;
        srclen -= 8;
        multiblock = (flags & 0x1800) != 0x1800;
        if (multiblock) {
            if (srclen < 4) {
                ret = AVERROR_INVALIDDATA;
                goto fail;
            }
            blocksize = AV_RL32(src);
            src      += 4;
            srclen   -= 4;
        } else
            blocksize = srclen;
        if (blocksize > srclen) {
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        tmp = av_realloc(dst, dstlen + blocksize + 32);
        if (!tmp) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        dst     = tmp;
        dstlen += blocksize + 32;
        AV_WL32(dst + offset, MKTAG('w', 'v', 'p', 'k'));   
        AV_WL32(dst + offset +  4, blocksize + 24);         
        AV_WL16(dst + offset +  8, ver);                    
        AV_WL16(dst + offset + 10, 0);                      
        AV_WL32(dst + offset + 12, 0);                      
        AV_WL32(dst + offset + 16, 0);                      
        AV_WL32(dst + offset + 20, samples);                
        AV_WL32(dst + offset + 24, flags);                  
        AV_WL32(dst + offset + 28, crc);                    
        memcpy(dst + offset + 32, src, blocksize);          
        src    += blocksize;
        srclen -= blocksize;
        offset += blocksize + 32;
    }
    *pdst = dst;
    *size = dstlen;
    return 0;
fail:
    av_freep(&dst);
    return ret;
}
