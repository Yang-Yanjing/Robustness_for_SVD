static int decode_p_block(FourXContext *f, uint16_t *dst, const uint16_t *src,
                          int log2w, int log2h, int stride)
{
    int index, h, code, ret, scale = 1;
    uint16_t *start, *end;
    unsigned dc = 0;
    av_assert0(log2w >= 0 && log2h >= 0);
    index = size2index[log2h][log2w];
    av_assert0(index >= 0);
    h     = 1 << log2h;
    code  = get_vlc2(&f->gb, block_type_vlc[1 - (f->version > 1)][index].table,
                     BLOCK_TYPE_VLC_BITS, 1);
    av_assert0(code >= 0 && code <= 6);
    start = f->last_frame_buffer;
    end   = start + stride * (f->avctx->height - h + 1) - (1 << log2w);
    if (code == 1) {
        log2h--;
        if ((ret = decode_p_block(f, dst, src, log2w, log2h, stride)) < 0)
            return ret;
        return decode_p_block(f, dst + (stride << log2h),
                              src + (stride << log2h),
                              log2w, log2h, stride);
    } else if (code == 2) {
        log2w--;
        if ((ret = decode_p_block(f, dst , src, log2w, log2h, stride)) < 0)
            return ret;
        return decode_p_block(f, dst + (1 << log2w),
                              src + (1 << log2w),
                              log2w, log2h, stride);
    } else if (code == 6) {
        if (bytestream2_get_bytes_left(&f->g2) < 4) {
            av_log(f->avctx, AV_LOG_ERROR, "wordstream overread\n");
            return AVERROR_INVALIDDATA;
        }
        if (log2w) {
            dst[0]      = bytestream2_get_le16u(&f->g2);
            dst[1]      = bytestream2_get_le16u(&f->g2);
        } else {
            dst[0]      = bytestream2_get_le16u(&f->g2);
            dst[stride] = bytestream2_get_le16u(&f->g2);
        }
        return 0;
    }
    if ((code&3)==0 && bytestream2_get_bytes_left(&f->g) < 1) {
        av_log(f->avctx, AV_LOG_ERROR, "bytestream overread\n");
        return AVERROR_INVALIDDATA;
    }
    if (code == 0) {
        src  += f->mv[bytestream2_get_byte(&f->g)];
    } else if (code == 3 && f->version >= 2) {
        return 0;
    } else if (code == 4) {
        src  += f->mv[bytestream2_get_byte(&f->g)];
        if (bytestream2_get_bytes_left(&f->g2) < 2){
            av_log(f->avctx, AV_LOG_ERROR, "wordstream overread\n");
            return AVERROR_INVALIDDATA;
        }
        dc    = bytestream2_get_le16(&f->g2);
    } else if (code == 5) {
        if (bytestream2_get_bytes_left(&f->g2) < 2){
            av_log(f->avctx, AV_LOG_ERROR, "wordstream overread\n");
            return AVERROR_INVALIDDATA;
        }
        av_assert0(start <= src && src <= end);
        scale = 0;
        dc    = bytestream2_get_le16(&f->g2);
    }
    if (start > src || src > end) {
        av_log(f->avctx, AV_LOG_ERROR, "mv out of pic\n");
        return AVERROR_INVALIDDATA;
    }
    mcdc(dst, src, log2w, h, stride, scale, dc);
    return 0;
}
