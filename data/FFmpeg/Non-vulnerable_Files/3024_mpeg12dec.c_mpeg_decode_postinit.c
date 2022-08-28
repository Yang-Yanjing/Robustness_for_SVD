static int mpeg_decode_postinit(AVCodecContext *avctx)
{
    Mpeg1Context *s1  = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    uint8_t old_permutation[64];
    int ret;
    if (avctx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        
        avctx->sample_aspect_ratio = av_d2q(1.0 / ff_mpeg1_aspect[s->aspect_ratio_info], 255);
    } else { 
        
        if (s->aspect_ratio_info > 1) {
            AVRational dar =
                av_mul_q(av_div_q(ff_mpeg2_aspect[s->aspect_ratio_info],
                                  (AVRational) { s1->pan_scan.width,
                                                 s1->pan_scan.height }),
                         (AVRational) { s->width, s->height });
            



            if ((s1->pan_scan.width == 0) || (s1->pan_scan.height == 0) ||
                (av_cmp_q(dar, (AVRational) { 4, 3 }) &&
                 av_cmp_q(dar, (AVRational) { 16, 9 }))) {
                s->avctx->sample_aspect_ratio =
                    av_div_q(ff_mpeg2_aspect[s->aspect_ratio_info],
                             (AVRational) { s->width, s->height });
            } else {
                s->avctx->sample_aspect_ratio =
                    av_div_q(ff_mpeg2_aspect[s->aspect_ratio_info],
                             (AVRational) { s1->pan_scan.width, s1->pan_scan.height });




                ff_dlog(avctx, "aspect A %d/%d\n",
                        ff_mpeg2_aspect[s->aspect_ratio_info].num,
                        ff_mpeg2_aspect[s->aspect_ratio_info].den);
                ff_dlog(avctx, "aspect B %d/%d\n", s->avctx->sample_aspect_ratio.num,
                        s->avctx->sample_aspect_ratio.den);
            }
        } else {
            s->avctx->sample_aspect_ratio =
                ff_mpeg2_aspect[s->aspect_ratio_info];
        }
    } 
