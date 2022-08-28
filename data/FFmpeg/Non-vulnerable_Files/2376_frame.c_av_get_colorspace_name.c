const char *av_get_colorspace_name(enum AVColorSpace val)
{
    static const char * const name[] = {
        [AVCOL_SPC_RGB]       = "GBR",
        [AVCOL_SPC_BT709]     = "bt709",
        [AVCOL_SPC_FCC]       = "fcc",
        [AVCOL_SPC_BT470BG]   = "bt470bg",
        [AVCOL_SPC_SMPTE170M] = "smpte170m",
        [AVCOL_SPC_SMPTE240M] = "smpte240m",
        [AVCOL_SPC_YCOCG]     = "YCgCo",
    };
    if ((unsigned)val >= FF_ARRAY_ELEMS(name))
        return NULL;
    return name[val];
}
