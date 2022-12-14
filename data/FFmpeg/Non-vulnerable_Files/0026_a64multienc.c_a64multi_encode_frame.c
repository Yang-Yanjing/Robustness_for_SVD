static int a64multi_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                                 const AVFrame *p, int *got_packet)
{
    A64Context *c = avctx->priv_data;
    int frame;
    int x, y;
    int b_height;
    int b_width;
    int req_size, ret;
    uint8_t *buf = NULL;
    int *charmap     = c->mc_charmap;
    uint8_t *colram  = c->mc_colram;
    uint8_t *charset = c->mc_charset;
    int *meta        = c->mc_meta_charset;
    int *best_cb     = c->mc_best_cb;
    int charset_size = 0x800 * (INTERLACED + 1);
    int colram_size  = 0x100 * c->mc_use_5col;
    int screen_size;
    if(CROP_SCREENS) {
        b_height = FFMIN(avctx->height,C64YRES) >> 3;
        b_width  = FFMIN(avctx->width ,C64XRES) >> 3;
        screen_size = b_width * b_height;
    } else {
        b_height = C64YRES >> 3;
        b_width  = C64XRES >> 3;
        screen_size = 0x400;
    }
    
    if (!p) {
        
        if (!c->mc_lifetime) return 0;
        
        if (!c->mc_frame_counter) {
            c->mc_lifetime = 0;
        }
        
        else c->mc_lifetime = c->mc_frame_counter;
    
    } else {
        
        if (c->mc_frame_counter < c->mc_lifetime) {
            to_meta_with_crop(avctx, p, meta + 32000 * c->mc_frame_counter);
            c->mc_frame_counter++;
            if (c->next_pts == AV_NOPTS_VALUE)
                c->next_pts = p->pts;
            
            return 0;
        }
    }
    
    if (c->mc_frame_counter == c->mc_lifetime) {
        req_size = 0;
        
        if (c->mc_lifetime) {
            int alloc_size = charset_size + c->mc_lifetime*(screen_size + colram_size);
            if ((ret = ff_alloc_packet2(avctx, pkt, alloc_size, 0)) < 0)
                return ret;
            buf = pkt->data;
            
            ret = avpriv_init_elbg(meta, 32, 1000 * c->mc_lifetime, best_cb,
                               CHARSET_CHARS, 50, charmap, &c->randctx);
            if (ret < 0)
                return ret;
            ret = avpriv_do_elbg(meta, 32, 1000 * c->mc_lifetime, best_cb,
                             CHARSET_CHARS, 50, charmap, &c->randctx);
            if (ret < 0)
                return ret;
            
            render_charset(avctx, charset, colram);
            
            memcpy(buf, charset, charset_size);
            
            buf      += charset_size;
            req_size += charset_size;
        }
        
        for (frame = 0; frame < c->mc_lifetime; frame++) {
            
            for (y = 0; y < b_height; y++) {
                for (x = 0; x < b_width; x++) {
                    buf[y * b_width + x] = charmap[y * b_width + x];
                }
            }
            
            buf += screen_size;
            req_size += screen_size;
            
            if (c->mc_use_5col) {
                a64_compress_colram(buf, charmap, colram);
                
                buf += colram_size;
                req_size += colram_size;
            }
            
            charmap += 1000;
        }
        AV_WB32(avctx->extradata + 4,  c->mc_frame_counter);
        AV_WB32(avctx->extradata + 8,  charset_size);
        AV_WB32(avctx->extradata + 12, screen_size + colram_size);
        
        c->mc_frame_counter = 0;
        pkt->pts = pkt->dts = c->next_pts;
        c->next_pts         = AV_NOPTS_VALUE;
        av_assert0(pkt->size >= req_size);
        pkt->size   = req_size;
        pkt->flags |= AV_PKT_FLAG_KEY;
        *got_packet = !!req_size;
    }
    return 0;
}
