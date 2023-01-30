}
static void GetAttrStrFromFontStack(font_stack_t **p_fonts,
        bool b_bold, bool b_italic, bool b_uline,
        CFRange p_range, CFMutableAttributedStringRef p_attrString)
{
    char       *psz_fontname = NULL;
    int         i_font_size  = 0;
    uint32_t    i_font_color = 0;
    if (VLC_SUCCESS == PeekFont(p_fonts, &psz_fontname, &i_font_size,
                                &i_font_color)) {
        setFontAttibutes(psz_fontname,
                         i_font_size,
                         i_font_color,
                         b_bold, b_italic, b_uline,
                         p_range,
                         p_attrString);
    }
}
