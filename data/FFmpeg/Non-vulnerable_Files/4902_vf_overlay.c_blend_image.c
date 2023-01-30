static void blend_image(AVFilterContext *ctx,
                        AVFrame *dst, const AVFrame *src,
                        int x, int y)
{
    OverlayContext *s = ctx->priv;
    int i, imax, j, jmax, k, kmax;
    const int src_w = src->width;
    const int src_h = src->height;
    const int dst_w = dst->width;
    const int dst_h = dst->height;
    if (x >= dst_w || x+src_w < 0 ||
        y >= dst_h || y+src_h < 0)
        return; 
    if (s->main_is_packed_rgb) {
        uint8_t alpha;          
        const int dr = s->main_rgba_map[R];
        const int dg = s->main_rgba_map[G];
        const int db = s->main_rgba_map[B];
        const int da = s->main_rgba_map[A];
        const int dstep = s->main_pix_step[0];
        const int sr = s->overlay_rgba_map[R];
        const int sg = s->overlay_rgba_map[G];
        const int sb = s->overlay_rgba_map[B];
        const int sa = s->overlay_rgba_map[A];
        const int sstep = s->overlay_pix_step[0];
        const int main_has_alpha = s->main_has_alpha;
        uint8_t *s, *sp, *d, *dp;
        i = FFMAX(-y, 0);
        sp = src->data[0] + i     * src->linesize[0];
        dp = dst->data[0] + (y+i) * dst->linesize[0];
        for (imax = FFMIN(-y + dst_h, src_h); i < imax; i++) {
            j = FFMAX(-x, 0);
            s = sp + j     * sstep;
            d = dp + (x+j) * dstep;
            for (jmax = FFMIN(-x + dst_w, src_w); j < jmax; j++) {
                alpha = s[sa];
                
                
                if (main_has_alpha && alpha != 0 && alpha != 255) {
                    uint8_t alpha_d = d[da];
                    alpha = UNPREMULTIPLY_ALPHA(alpha, alpha_d);
                }
                switch (alpha) {
                case 0:
                    break;
                case 255:
                    d[dr] = s[sr];
                    d[dg] = s[sg];
                    d[db] = s[sb];
                    break;
                default:
                    
                    
                    d[dr] = FAST_DIV255(d[dr] * (255 - alpha) + s[sr] * alpha);
                    d[dg] = FAST_DIV255(d[dg] * (255 - alpha) + s[sg] * alpha);
                    d[db] = FAST_DIV255(d[db] * (255 - alpha) + s[sb] * alpha);
                }
                if (main_has_alpha) {
                    switch (alpha) {
                    case 0:
                        break;
                    case 255:
                        d[da] = s[sa];
                        break;
                    default:
                        
                        d[da] += FAST_DIV255((255 - d[da]) * s[sa]);
                    }
                }
                d += dstep;
                s += sstep;
            }
            dp += dst->linesize[0];
            sp += src->linesize[0];
        }
    } else {
        const int main_has_alpha = s->main_has_alpha;
        if (main_has_alpha) {
            uint8_t alpha;          
            uint8_t *s, *sa, *d, *da;
            i = FFMAX(-y, 0);
            sa = src->data[3] + i     * src->linesize[3];
            da = dst->data[3] + (y+i) * dst->linesize[3];
            for (imax = FFMIN(-y + dst_h, src_h); i < imax; i++) {
                j = FFMAX(-x, 0);
                s = sa + j;
                d = da + x+j;
                for (jmax = FFMIN(-x + dst_w, src_w); j < jmax; j++) {
                    alpha = *s;
                    if (alpha != 0 && alpha != 255) {
                        uint8_t alpha_d = *d;
                        alpha = UNPREMULTIPLY_ALPHA(alpha, alpha_d);
                    }
                    switch (alpha) {
                    case 0:
                        break;
                    case 255:
                        *d = *s;
                        break;
                    default:
                        
                        *d += FAST_DIV255((255 - *d) * *s);
                    }
                    d += 1;
                    s += 1;
                }
                da += dst->linesize[3];
                sa += src->linesize[3];
            }
        }
        for (i = 0; i < 3; i++) {
            int hsub = i ? s->hsub : 0;
            int vsub = i ? s->vsub : 0;
            int src_wp = FF_CEIL_RSHIFT(src_w, hsub);
            int src_hp = FF_CEIL_RSHIFT(src_h, vsub);
            int dst_wp = FF_CEIL_RSHIFT(dst_w, hsub);
            int dst_hp = FF_CEIL_RSHIFT(dst_h, vsub);
            int yp = y>>vsub;
            int xp = x>>hsub;
            uint8_t *s, *sp, *d, *dp, *a, *ap;
            j = FFMAX(-yp, 0);
            sp = src->data[i] + j         * src->linesize[i];
            dp = dst->data[i] + (yp+j)    * dst->linesize[i];
            ap = src->data[3] + (j<<vsub) * src->linesize[3];
            for (jmax = FFMIN(-yp + dst_hp, src_hp); j < jmax; j++) {
                k = FFMAX(-xp, 0);
                d = dp + xp+k;
                s = sp + k;
                a = ap + (k<<hsub);
                for (kmax = FFMIN(-xp + dst_wp, src_wp); k < kmax; k++) {
                    int alpha_v, alpha_h, alpha;
                    
                    if (hsub && vsub && j+1 < src_hp && k+1 < src_wp) {
                        alpha = (a[0] + a[src->linesize[3]] +
                                 a[1] + a[src->linesize[3]+1]) >> 2;
                    } else if (hsub || vsub) {
                        alpha_h = hsub && k+1 < src_wp ?
                            (a[0] + a[1]) >> 1 : a[0];
                        alpha_v = vsub && j+1 < src_hp ?
                            (a[0] + a[src->linesize[3]]) >> 1 : a[0];
                        alpha = (alpha_v + alpha_h) >> 1;
                    } else
                        alpha = a[0];
                    
                    
                    if (main_has_alpha && alpha != 0 && alpha != 255) {
                        
                        uint8_t alpha_d;
                        if (hsub && vsub && j+1 < src_hp && k+1 < src_wp) {
                            alpha_d = (d[0] + d[src->linesize[3]] +
                                       d[1] + d[src->linesize[3]+1]) >> 2;
                        } else if (hsub || vsub) {
                            alpha_h = hsub && k+1 < src_wp ?
                                (d[0] + d[1]) >> 1 : d[0];
                            alpha_v = vsub && j+1 < src_hp ?
                                (d[0] + d[src->linesize[3]]) >> 1 : d[0];
                            alpha_d = (alpha_v + alpha_h) >> 1;
                        } else
                            alpha_d = d[0];
                        alpha = UNPREMULTIPLY_ALPHA(alpha, alpha_d);
                    }
                    *d = FAST_DIV255(*d * (255 - alpha) + *s * alpha);
                    s++;
                    d++;
                    a += 1 << hsub;
                }
                dp += dst->linesize[i];
                sp += src->linesize[i];
                ap += (1 << vsub) * src->linesize[3];
            }
        }
    }
}
