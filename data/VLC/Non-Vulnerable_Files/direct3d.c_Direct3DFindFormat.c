 * It returns the format (closest to chroma) that can be converted to target */
static const d3d_format_t *Direct3DFindFormat(vout_display_t *vd, vlc_fourcc_t chroma, D3DFORMAT target)
{
    vout_display_sys_t *sys = vd->sys;
    bool hardware_scale_ok = !(vd->fmt.i_visible_width & 1) && !(vd->fmt.i_visible_height & 1);
    for (unsigned pass = 0; pass < 2; pass++) {
        const vlc_fourcc_t *list;
        if (pass == 0 && hardware_scale_ok && sys->allow_hw_yuv && vlc_fourcc_IsYUV(chroma))
            list = vlc_fourcc_GetYUVFallback(chroma);
        else if (pass == 1)
            list = vlc_fourcc_GetRGBFallback(chroma);
        else
            continue;
        for (unsigned i = 0; list[i] != 0; i++) {
            for (unsigned j = 0; d3d_formats[j].name; j++) {
                const d3d_format_t *format = &d3d_formats[j];
                if (format->fourcc != list[i])
                    continue;
                msg_Warn(vd, "trying surface pixel format: %s",
                         format->name);
                if (!Direct3DCheckConversion(vd, format->format, target)) {
                    msg_Dbg(vd, "selected surface pixel format is %s",
                            format->name);
                    return format;
                }
            }
        }
    }
    return NULL;
}
