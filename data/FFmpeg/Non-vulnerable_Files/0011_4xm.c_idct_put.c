static inline void idct_put(FourXContext *f, int x, int y)
{
    int16_t (*block)[64] = f->block;
    int stride           = f->avctx->width;
    int i;
    uint16_t *dst = f->frame_buffer + y * stride + x;
    for (i = 0; i < 4; i++) {
        block[i][0] += 0x80 * 8 * 8;
        idct(block[i]);
    }
    if (!(f->avctx->flags & AV_CODEC_FLAG_GRAY)) {
        for (i = 4; i < 6; i++)
            idct(block[i]);
    }
    



    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int16_t *temp = block[(x >> 2) + 2 * (y >> 2)] +
                            2 * (x & 3) + 2 * 8 * (y & 3); 
            int cb = block[4][x + 8 * y];
            int cr = block[5][x + 8 * y];
            int cg = (cb + cr) >> 1;
            int y;
            cb += cb;
            y               = temp[0];
            dst[0]          = ((y + cb) >> 3) + (((y - cg) & 0xFC) << 3) + (((y + cr) & 0xF8) << 8);
            y               = temp[1];
            dst[1]          = ((y + cb) >> 3) + (((y - cg) & 0xFC) << 3) + (((y + cr) & 0xF8) << 8);
            y               = temp[8];
            dst[stride]     = ((y + cb) >> 3) + (((y - cg) & 0xFC) << 3) + (((y + cr) & 0xF8) << 8);
            y               = temp[9];
            dst[1 + stride] = ((y + cb) >> 3) + (((y - cg) & 0xFC) << 3) + (((y + cr) & 0xF8) << 8);
            dst            += 2;
        }
        dst += 2 * stride - 2 * 8;
    }
}
