static void fill_picture_rgb(AVFilterContext *ctx, AVFrame *picref)
{
    LifeContext *life = ctx->priv;
    uint8_t *buf = life->buf[life->buf_idx];
    int i, j;
    
    for (i = 0; i < life->h; i++) {
        uint8_t *p = picref->data[0] + i * picref->linesize[0];
        for (j = 0; j < life->w; j++) {
            uint8_t v = buf[i*life->w + j];
            if (life->mold && v != ALIVE_CELL) {
                const uint8_t *c1 = life-> mold_color;
                const uint8_t *c2 = life->death_color;
                int death_age = FFMIN((0xff - v) * life->mold, 0xff);
                *p++ = FAST_DIV255((c2[0] << 8) + ((int)c1[0] - (int)c2[0]) * death_age);
                *p++ = FAST_DIV255((c2[1] << 8) + ((int)c1[1] - (int)c2[1]) * death_age);
                *p++ = FAST_DIV255((c2[2] << 8) + ((int)c1[2] - (int)c2[2]) * death_age);
            } else {
                const uint8_t *c = v == ALIVE_CELL ? life->life_color : life->death_color;
                AV_WB24(p, c[0]<<16 | c[1]<<8 | c[2]);
                p += 3;
            }
        }
    }
}
