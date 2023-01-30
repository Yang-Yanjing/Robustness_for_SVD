} dx_format_t;
static DWORD DirectXGetFourcc(vlc_fourcc_t codec)
{
    static const dx_format_t dx_formats[] = {
        { VLC_CODEC_YUYV, MAKEFOURCC('Y','U','Y','2') },
        { VLC_CODEC_UYVY, MAKEFOURCC('U','Y','V','Y') },
        { VLC_CODEC_YVYU, MAKEFOURCC('Y','V','Y','U') },
        { VLC_CODEC_YV12, MAKEFOURCC('Y','V','1','2') },
        { VLC_CODEC_I420, MAKEFOURCC('Y','V','1','2') },
        { VLC_CODEC_J420, MAKEFOURCC('Y','V','1','2') },
        { 0, 0 }
    };
    for (unsigned i = 0; dx_formats[i].codec != 0; i++) {
        if (dx_formats[i].codec == codec)
            return dx_formats[i].fourcc;
    }
    return 0;
}
