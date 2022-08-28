static void vectorscope(VectorscopeContext *s, AVFrame *in, AVFrame *out, int pd)
{
    const uint8_t * const *src = (const uint8_t * const *)in->data;
    const int slinesizex = in->linesize[s->x];
    const int slinesizey = in->linesize[s->y];
    const int slinesized = in->linesize[pd];
    const int dlinesize = out->linesize[0];
    const int intensity = s->intensity;
    const int px = s->x, py = s->y;
    const int h = s->planeheight[py];
    const int w = s->planewidth[px];
    const uint8_t *spx = src[px];
    const uint8_t *spy = src[py];
    const uint8_t *spd = src[pd];
    const int hsub = s->hsub;
    const int vsub = s->vsub;
    uint8_t **dst = out->data;
    uint8_t *dpx = dst[px];
    uint8_t *dpy = dst[py];
    uint8_t *dpd = dst[pd];
    int i, j;
    switch (s->mode) {
    case COLOR:
    case GRAY:
        if (s->is_yuv) {
            for (i = 0; i < h; i++) {
                const int iwx = i * slinesizex;
                const int iwy = i * slinesizey;
                for (j = 0; j < w; j++) {
                    const int x = spx[iwx + j];
                    const int y = spy[iwy + j];
                    const int pos = y * dlinesize + x;
                    dpd[pos] = FFMIN(dpd[pos] + intensity, 255);
                    if (dst[3])
                        dst[3][pos] = 255;
                }
            }
        } else {
            for (i = 0; i < h; i++) {
                const int iwx = i * slinesizex;
                const int iwy = i * slinesizey;
                for (j = 0; j < w; j++) {
                    const int x = spx[iwx + j];
                    const int y = spy[iwy + j];
                    const int pos = y * dlinesize + x;
                    dst[0][pos] = FFMIN(dst[0][pos] + intensity, 255);
                    dst[1][pos] = FFMIN(dst[1][pos] + intensity, 255);
                    dst[2][pos] = FFMIN(dst[2][pos] + intensity, 255);
                    if (dst[3])
                        dst[3][pos] = 255;
                }
            }
        }
        break;
    case COLOR2:
        if (s->is_yuv) {
            for (i = 0; i < h; i++) {
                const int iw1 = i * slinesizex;
                const int iw2 = i * slinesizey;
                for (j = 0; j < w; j++) {
                    const int x = spx[iw1 + j];
                    const int y = spy[iw2 + j];
                    const int pos = y * dlinesize + x;
                    if (!dpd[pos])
                        dpd[pos] = FFABS(128 - x) + FFABS(128 - y);
                    dpx[pos] = x;
                    dpy[pos] = y;
                    if (dst[3])
                        dst[3][pos] = 255;
                }
            }
        } else {
            for (i = 0; i < h; i++) {
                const int iw1 = i * slinesizex;
                const int iw2 = i * slinesizey;
                for (j = 0; j < w; j++) {
                    const int x = spx[iw1 + j];
                    const int y = spy[iw2 + j];
                    const int pos = y * dlinesize + x;
                    if (!dpd[pos])
                        dpd[pos] = FFMIN(x + y, 255);
                    dpx[pos] = x;
                    dpy[pos] = y;
                    if (dst[3])
                        dst[3][pos] = 255;
                }
            }
        }
        break;
    case COLOR3:
        for (i = 0; i < h; i++) {
            const int iw1 = i * slinesizex;
            const int iw2 = i * slinesizey;
            for (j = 0; j < w; j++) {
                const int x = spx[iw1 + j];
                const int y = spy[iw2 + j];
                const int pos = y * dlinesize + x;
                dpd[pos] = FFMIN(255, dpd[pos] + intensity);
                dpx[pos] = x;
                dpy[pos] = y;
                if (dst[3])
                    dst[3][pos] = 255;
            }
        }
        break;
    case COLOR4:
        for (i = 0; i < in->height; i++) {
            const int iwx = (i >> vsub) * slinesizex;
            const int iwy = (i >> vsub) * slinesizey;
            const int iwd = i * slinesized;
            for (j = 0; j < in->width; j++) {
                const int x = spx[iwx + (j >> hsub)];
                const int y = spy[iwy + (j >> hsub)];
                const int pos = y * dlinesize + x;
                dpd[pos] = FFMAX(spd[iwd + j], dpd[pos]);
                dpx[pos] = x;
                dpy[pos] = y;
                if (dst[3])
                    dst[3][pos] = 255;
            }
        }
        break;
    default:
        av_assert0(0);
    }
    envelope(s, out);
    if (s->mode == COLOR) {
        for (i = 0; i < out->height; i++) {
            for (j = 0; j < out->width; j++) {
                if (!dpd[i * out->linesize[pd] + j]) {
                    dpx[i * out->linesize[px] + j] = j;
                    dpy[i * out->linesize[py] + j] = i;
                    dpd[i * out->linesize[pd] + j] = 128;
                }
            }
        }
    }
}
