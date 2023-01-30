 * It is used as pf_add_string callback in the vout method by this module */
static int RenderText(filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list)
{
    filter_sys_t *p_sys = p_filter->p_sys;
    char         *psz_string;
    int           i_font_size;
    uint32_t      i_font_color;
    bool          b_bold, b_uline, b_italic;
    vlc_value_t val;
    b_bold = b_uline = b_italic = FALSE;
    VLC_UNUSED(p_chroma_list);
    p_sys->i_font_size = GetFontSize(p_filter);
    // Sanity check
    if (!p_region_in || !p_region_out)
        return VLC_EGENERIC;
    psz_string = p_region_in->psz_text;
    if (!psz_string || !*psz_string)
        return VLC_EGENERIC;
    if (p_region_in->p_style) {
        i_font_color = VLC_CLIP(p_region_in->p_style->i_font_color, 0, 0xFFFFFF);
        i_font_size  = VLC_CLIP(p_region_in->p_style->i_font_size, 0, 255);
        if (p_region_in->p_style->i_style_flags) {
            if (p_region_in->p_style->i_style_flags & STYLE_BOLD)
                b_bold = TRUE;
            if (p_region_in->p_style->i_style_flags & STYLE_ITALIC)
                b_italic = TRUE;
            if (p_region_in->p_style->i_style_flags & STYLE_UNDERLINE)
                b_uline = TRUE;
        }
    } else {
        i_font_color = p_sys->i_font_color;
        i_font_size  = p_sys->i_font_size;
    }
    if (i_font_size <= 0) {
        msg_Warn(p_filter, "invalid fontsize, using 12");
        if (VLC_SUCCESS == var_Get(p_filter, "scale", &val))
            i_font_size = 12 * val.i_int / 1000;
        else
            i_font_size = 12;
    }
    p_region_out->i_x = p_region_in->i_x;
    p_region_out->i_y = p_region_in->i_y;
    CFMutableAttributedStringRef p_attrString = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
    if (p_attrString) {
        CFStringRef   p_cfString;
        int           len;
        EliminateCRLF(psz_string);
        p_cfString = CFStringCreateWithCString(NULL, psz_string, kCFStringEncodingUTF8);
        CFAttributedStringReplaceString(p_attrString, CFRangeMake(0, 0), p_cfString);
        CFRelease(p_cfString);
        len = CFAttributedStringGetLength(p_attrString);
        setFontAttibutes(p_sys->psz_font_name, i_font_size, i_font_color, b_bold, b_italic, b_uline,
                                             CFRangeMake(0, len), p_attrString);
        RenderYUVA(p_filter, p_region_out, p_attrString);
        CFRelease(p_attrString);
    }
    return VLC_SUCCESS;
}
