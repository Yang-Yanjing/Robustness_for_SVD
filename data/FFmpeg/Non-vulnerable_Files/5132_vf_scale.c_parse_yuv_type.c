static const int *parse_yuv_type(const char *s, enum AVColorSpace colorspace)
{
    if (!s)
        s = "bt601";
    if (s && strstr(s, "bt709")) {
        colorspace = AVCOL_SPC_BT709;
    } else if (s && strstr(s, "fcc")) {
        colorspace = AVCOL_SPC_FCC;
    } else if (s && strstr(s, "smpte240m")) {
        colorspace = AVCOL_SPC_SMPTE240M;
    } else if (s && (strstr(s, "bt601") || strstr(s, "bt470") || strstr(s, "smpte170m"))) {
        colorspace = AVCOL_SPC_BT470BG;
    }
    if (colorspace < 1 || colorspace > 7) {
        colorspace = AVCOL_SPC_BT470BG;
    }
    return sws_getCoefficients(colorspace);
}
