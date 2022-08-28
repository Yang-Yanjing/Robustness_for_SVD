static int mpeg_decode_a53_cc(AVCodecContext *avctx,
                              const uint8_t *p, int buf_size)
{
    Mpeg1Context *s1 = avctx->priv_data;
    if (buf_size >= 6 &&
        p[0] == 'G' && p[1] == 'A' && p[2] == '9' && p[3] == '4' &&
        p[4] == 3 && (p[5] & 0x40)) {
        
        int cc_count = p[5] & 0x1f;
        if (cc_count > 0 && buf_size >= 7 + cc_count * 3) {
            av_freep(&s1->a53_caption);
            s1->a53_caption_size = cc_count * 3;
            s1->a53_caption      = av_malloc(s1->a53_caption_size);
            if (s1->a53_caption)
                memcpy(s1->a53_caption, p + 7, s1->a53_caption_size);
        }
        return 1;
    } else if (buf_size >= 11 &&
               p[0] == 'C' && p[1] == 'C' && p[2] == 0x01 && p[3] == 0xf8) {
        
        int cc_count = 0;
        int i;
        
        
        for (i = 5; i + 6 <= buf_size && ((p[i] & 0xfe) == 0xfe); i += 6)
            cc_count++;
        
        if (cc_count > 0) {
            av_freep(&s1->a53_caption);
            s1->a53_caption_size = cc_count * 6;
            s1->a53_caption      = av_malloc(s1->a53_caption_size);
            if (s1->a53_caption) {
                uint8_t field1 = !!(p[4] & 0x80);
                uint8_t *cap = s1->a53_caption;
                p += 5;
                for (i = 0; i < cc_count; i++) {
                    cap[0] = (p[0] == 0xff && field1) ? 0xfc : 0xfd;
                    cap[1] = p[1];
                    cap[2] = p[2];
                    cap[3] = (p[3] == 0xff && !field1) ? 0xfc : 0xfd;
                    cap[4] = p[4];
                    cap[5] = p[5];
                    cap += 6;
                    p += 6;
                }
            }
        }
        return 1;
    }
    return 0;
}
