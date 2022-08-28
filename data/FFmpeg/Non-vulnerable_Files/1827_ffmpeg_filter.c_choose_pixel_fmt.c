enum AVPixelFormat choose_pixel_fmt(AVStream *st, AVCodecContext *enc_ctx, AVCodec *codec, enum AVPixelFormat target)
{
    if (codec && codec->pix_fmts) {
        const enum AVPixelFormat *p = codec->pix_fmts;
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(target);
        int has_alpha = desc ? desc->nb_components % 2 == 0 : 0;
        enum AVPixelFormat best= AV_PIX_FMT_NONE;
        if (enc_ctx->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
            p = get_compliance_unofficial_pix_fmts(enc_ctx->codec_id, p);
        }
        for (; *p != AV_PIX_FMT_NONE; p++) {
            best= avcodec_find_best_pix_fmt_of_2(best, *p, target, has_alpha, NULL);
            if (*p == target)
                break;
        }
        if (*p == AV_PIX_FMT_NONE) {
            if (target != AV_PIX_FMT_NONE)
                av_log(NULL, AV_LOG_WARNING,
                       "Incompatible pixel format '%s' for codec '%s', auto-selecting format '%s'\n",
                       av_get_pix_fmt_name(target),
                       codec->name,
                       av_get_pix_fmt_name(best));
            return best;
        }
    }
    return target;
}
