int ff_dct_quantize_c(MpegEncContext *s,
                        int16_t *block, int n,
                        int qscale, int *overflow)
{
    int i, j, level, last_non_zero, q, start_i;
    const int *qmat;
    const uint8_t *scantable= s->intra_scantable.scantable;
    int bias;
    int max=0;
    unsigned int threshold1, threshold2;
    s->fdsp.fdct(block);
    if(s->dct_error_sum)
        s->denoise_dct(s, block);
    if (s->mb_intra) {
        if (!s->h263_aic) {
            if (n < 4)
                q = s->y_dc_scale;
            else
                q = s->c_dc_scale;
            q = q << 3;
        } else
            
            q = 1 << 3;
        
        block[0] = (block[0] + (q >> 1)) / q;
        start_i = 1;
        last_non_zero = 0;
        qmat = n < 4 ? s->q_intra_matrix[qscale] : s->q_chroma_intra_matrix[qscale];
        bias= s->intra_quant_bias*(1<<(QMAT_SHIFT - QUANT_BIAS_SHIFT));
    } else {
        start_i = 0;
        last_non_zero = -1;
        qmat = s->q_inter_matrix[qscale];
        bias= s->inter_quant_bias*(1<<(QMAT_SHIFT - QUANT_BIAS_SHIFT));
    }
    threshold1= (1<<QMAT_SHIFT) - bias - 1;
    threshold2= (threshold1<<1);
    for(i=63;i>=start_i;i--) {
        j = scantable[i];
        level = block[j] * qmat[j];
        if(((unsigned)(level+threshold1))>threshold2){
            last_non_zero = i;
            break;
        }else{
            block[j]=0;
        }
    }
    for(i=start_i; i<=last_non_zero; i++) {
        j = scantable[i];
        level = block[j] * qmat[j];


        if(((unsigned)(level+threshold1))>threshold2){
            if(level>0){
                level= (bias + level)>>QMAT_SHIFT;
                block[j]= level;
            }else{
                level= (bias - level)>>QMAT_SHIFT;
                block[j]= -level;
            }
            max |=level;
        }else{
            block[j]=0;
        }
    }
    *overflow= s->max_qcoeff < max; 
    
    if (s->idsp.perm_type != FF_IDCT_PERM_NONE)
        block_permute(block, s->idsp.idct_permutation,
                      scantable, last_non_zero);
    return last_non_zero;
}
#define OFFSET(x) offsetof(MpegEncContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption h263_options[] = {
    { "obmc",         "use overlapped block motion compensation.", OFFSET(obmc), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    { "structured_slices","Write slice start position at every GOB header instead of just GOB number.", OFFSET(h263_slice_structured), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE},
    { "mb_info",      "emit macroblock info for RFC 2190 packetization, the parameter value is the maximum payload size", OFFSET(mb_info), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, INT_MAX, VE },
    FF_MPV_COMMON_OPTS
    { NULL },
};
static const AVClass h263_class = {
    .class_name = "H.263 encoder",
    .item_name  = av_default_item_name,
    .option     = h263_options,
    .version    = LIBAVUTIL_VERSION_INT,
};
AVCodec ff_h263_encoder = {
    .name           = "h263",
    .long_name      = NULL_IF_CONFIG_SMALL("H.263 / H.263-1996"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_H263,
    .priv_data_size = sizeof(MpegEncContext),
    .init           = ff_mpv_encode_init,
    .encode2        = ff_mpv_encode_picture,
    .close          = ff_mpv_encode_end,
    .pix_fmts= (const enum AVPixelFormat[]){AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE},
    .priv_class     = &h263_class,
};
static const AVOption h263p_options[] = {
    { "umv",        "Use unlimited motion vectors.",    OFFSET(umvplus), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    { "aiv",        "Use alternative inter VLC.",       OFFSET(alt_inter_vlc), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    { "obmc",       "use overlapped block motion compensation.", OFFSET(obmc), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE },
    { "structured_slices", "Write slice start position at every GOB header instead of just GOB number.", OFFSET(h263_slice_structured), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, VE},
    FF_MPV_COMMON_OPTS
    { NULL },
};
static const AVClass h263p_class = {
    .class_name = "H.263p encoder",
    .item_name  = av_default_item_name,
    .option     = h263p_options,
    .version    = LIBAVUTIL_VERSION_INT,
};
AVCodec ff_h263p_encoder = {
    .name           = "h263p",
    .long_name      = NULL_IF_CONFIG_SMALL("H.263+ / H.263-1998 / H.263 version 2"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_H263P,
    .priv_data_size = sizeof(MpegEncContext),
    .init           = ff_mpv_encode_init,
    .encode2        = ff_mpv_encode_picture,
    .close          = ff_mpv_encode_end,
    .capabilities   = AV_CODEC_CAP_SLICE_THREADS,
    .pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE },
    .priv_class     = &h263p_class,
};
static const AVClass msmpeg4v2_class = {
    .class_name = "msmpeg4v2 encoder",
    .item_name  = av_default_item_name,
    .option     = ff_mpv_generic_options,
    .version    = LIBAVUTIL_VERSION_INT,
};
AVCodec ff_msmpeg4v2_encoder = {
    .name           = "msmpeg4v2",
    .long_name      = NULL_IF_CONFIG_SMALL("MPEG-4 part 2 Microsoft variant version 2"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_MSMPEG4V2,
    .priv_data_size = sizeof(MpegEncContext),
    .init           = ff_mpv_encode_init,
    .encode2        = ff_mpv_encode_picture,
    .close          = ff_mpv_encode_end,
    .pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE },
    .priv_class     = &msmpeg4v2_class,
};
static const AVClass msmpeg4v3_class = {
    .class_name = "msmpeg4v3 encoder",
    .item_name  = av_default_item_name,
    .option     = ff_mpv_generic_options,
    .version    = LIBAVUTIL_VERSION_INT,
};
AVCodec ff_msmpeg4v3_encoder = {
    .name           = "msmpeg4",
    .long_name      = NULL_IF_CONFIG_SMALL("MPEG-4 part 2 Microsoft variant version 3"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_MSMPEG4V3,
    .priv_data_size = sizeof(MpegEncContext),
    .init           = ff_mpv_encode_init,
    .encode2        = ff_mpv_encode_picture,
    .close          = ff_mpv_encode_end,
    .pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE },
    .priv_class     = &msmpeg4v3_class,
};
static const AVClass wmv1_class = {
    .class_name = "wmv1 encoder",
    .item_name  = av_default_item_name,
    .option     = ff_mpv_generic_options,
    .version    = LIBAVUTIL_VERSION_INT,
};
AVCodec ff_wmv1_encoder = {
    .name           = "wmv1",
    .long_name      = NULL_IF_CONFIG_SMALL("Windows Media Video 7"),
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_WMV1,
    .priv_data_size = sizeof(MpegEncContext),
    .init           = ff_mpv_encode_init,
    .encode2        = ff_mpv_encode_picture,
    .close          = ff_mpv_encode_end,
    .pix_fmts       = (const enum AVPixelFormat[]){ AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE },
    .priv_class     = &wmv1_class,
};
