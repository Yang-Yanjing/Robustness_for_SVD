}
static int HandleFontAttributes(xml_reader_t *p_xml_reader,
                                  font_stack_t **p_fonts)
{
    int        rv;
    char      *psz_fontname = NULL;
    uint32_t   i_font_color = 0xffffff;
    int        i_font_alpha = 0;
    int        i_font_size  = 24;
    const char *attr, *value;
    /* Default all attributes to the top font in the stack -- in case not
     * all attributes are specified in the sub-font */
    if (VLC_SUCCESS == PeekFont(p_fonts,
                                &psz_fontname,
                                &i_font_size,
                                &i_font_color)) {
        if (psz_fontname)
            psz_fontname = strdup(psz_fontname);
        i_font_size = i_font_size;
    }
    i_font_alpha = (i_font_color >> 24) & 0xff;
    i_font_color &= 0x00ffffff;
    while ((attr = xml_ReaderNextAttr(p_xml_reader, &value))) {
        if (!strcasecmp("face", attr)) {
            free(psz_fontname);
            if (value)
                psz_fontname = strdup(value);
        } else if (!strcasecmp("size", attr)) {
            if ((*value == '+') || (*value == '-')) {
                int i_value = atoi(value);
                if ((i_value >= -5) && (i_value <= 5))
                    i_font_size += (i_value * i_font_size) / 10;
                else if (i_value < -5)
                    i_font_size = - i_value;
                else if (i_value > 5)
                    i_font_size = i_value;
            }
            else
                i_font_size = atoi(value);
        } else if (!strcasecmp("color", attr)) {
            if (value[0] == '#') {
                i_font_color = strtol(value + 1, NULL, 16);
                i_font_color &= 0x00ffffff;
            } else {
                /* color detection fallback */
                unsigned int count = sizeof(ppsz_color_names);
                for (unsigned x = 0; x < count; x++) {
                    if (!strcmp(value, ppsz_color_names[x])) {
                        i_font_color = pi_color_values[x];
                        break;
                    }
                }
            }
        } else if (!strcasecmp("alpha", attr) && (value[0] == '#')) {
            i_font_alpha = strtol(value + 1, NULL, 16);
            i_font_alpha &= 0xff;
        }
    }
    rv = PushFont(p_fonts,
                  psz_fontname,
                  i_font_size,
                  (i_font_color & 0xffffff) | ((i_font_alpha & 0xff) << 24));
    free(psz_fontname);
    return rv;
}
