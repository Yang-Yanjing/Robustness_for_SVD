int videotoolbox_init(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    int loglevel = (ist->hwaccel_id == HWACCEL_AUTO) ? AV_LOG_VERBOSE : AV_LOG_ERROR;
    int ret = 0;
    VTContext *vt;
    vt = av_mallocz(sizeof(*vt));
    if (!vt)
        return AVERROR(ENOMEM);
    ist->hwaccel_ctx           = vt;
    ist->hwaccel_uninit        = videotoolbox_uninit;
    ist->hwaccel_retrieve_data = videotoolbox_retrieve_data;
    vt->tmp_frame = av_frame_alloc();
    if (!vt->tmp_frame) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    if (ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX) {
#if CONFIG_VIDEOTOOLBOX
        if (!videotoolbox_pixfmt) {
            ret = av_videotoolbox_default_init(s);
        } else {
            AVVideotoolboxContext *vtctx = av_videotoolbox_alloc_context();
            CFStringRef pixfmt_str = CFStringCreateWithCString(kCFAllocatorDefault,
                                                               videotoolbox_pixfmt,
                                                               kCFStringEncodingUTF8);
#if HAVE_UTGETOSTYPEFROMSTRING
            vtctx->cv_pix_fmt_type = UTGetOSTypeFromString(pixfmt_str);
#else
            av_log(s, loglevel, "UTGetOSTypeFromString() is not available "
                   "on this platform, %s pixel format can not be honored from "
                   "the command line\n", videotoolbox_pixfmt);
#endif
            ret = av_videotoolbox_default_init2(s, vtctx);
            CFRelease(pixfmt_str);
        }
#endif
    } else {
#if CONFIG_VDA
        if (!videotoolbox_pixfmt) {
            ret = av_vda_default_init(s);
        } else {
            AVVDAContext *vdactx = av_vda_alloc_context();
            CFStringRef pixfmt_str = CFStringCreateWithCString(kCFAllocatorDefault,
                                                               videotoolbox_pixfmt,
                                                               kCFStringEncodingUTF8);
#if HAVE_UTGETOSTYPEFROMSTRING
            vdactx->cv_pix_fmt_type = UTGetOSTypeFromString(pixfmt_str);
#else
            av_log(s, loglevel, "UTGetOSTypeFromString() is not available "
                   "on this platform, %s pixel format can not be honored from "
                   "the command line\n", videotoolbox_pixfmt);
#endif
            ret = av_vda_default_init2(s, vdactx);
            CFRelease(pixfmt_str);
        }
#endif
    }
    if (ret < 0) {
        av_log(NULL, loglevel,
               "Error creating %s decoder.\n", ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX ? "Videotoolbox" : "VDA");
        goto fail;
    }
    return 0;
fail:
    videotoolbox_uninit(s);
    return ret;
}
