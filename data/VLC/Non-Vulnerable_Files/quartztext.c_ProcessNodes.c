}
static int ProcessNodes(filter_t *p_filter,
                         xml_reader_t *p_xml_reader,
                         text_style_t *p_font_style,
                         CFMutableAttributedStringRef p_attrString)
{
    int           rv             = VLC_SUCCESS;
    filter_sys_t *p_sys          = p_filter->p_sys;
    font_stack_t *p_fonts        = NULL;
    int type;
    const char *node;
    bool b_italic = false;
    bool b_bold   = false;
    bool b_uline  = false;
    if (p_font_style) {
        rv = PushFont(&p_fonts,
               p_font_style->psz_fontname,
               p_font_style->i_font_size,
               (p_font_style->i_font_color & 0xffffff) |
                   ((p_font_style->i_font_alpha & 0xff) << 24));
        if (p_font_style->i_style_flags & STYLE_BOLD)
            b_bold = true;
        if (p_font_style->i_style_flags & STYLE_ITALIC)
            b_italic = true;
        if (p_font_style->i_style_flags & STYLE_UNDERLINE)
            b_uline = true;
    } else {
        rv = PushFont(&p_fonts,
                       p_sys->psz_font_name,
                       p_sys->i_font_size,
                       p_sys->i_font_color);
    }
    if (rv != VLC_SUCCESS)
        return rv;
    while ((type = xml_ReaderNextNode(p_xml_reader, &node)) > 0) {
        switch (type) {
            case XML_READER_ENDELEM:
                if (!strcasecmp("font", node))
                    PopFont(&p_fonts);
                else if (!strcasecmp("b", node))
                    b_bold   = false;
                else if (!strcasecmp("i", node))
                    b_italic = false;
                else if (!strcasecmp("u", node))
                    b_uline  = false;
                break;
            case XML_READER_STARTELEM:
                if (!strcasecmp("font", node))
                    rv = HandleFontAttributes(p_xml_reader, &p_fonts);
                else if (!strcasecmp("b", node))
                    b_bold = true;
                else if (!strcasecmp("i", node))
                    b_italic = true;
                else if (!strcasecmp("u", node))
                    b_uline = true;
                else if (!strcasecmp("br", node)) {
                    CFMutableAttributedStringRef p_attrnode = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
                    CFAttributedStringReplaceString(p_attrnode, CFRangeMake(0, 0), CFSTR("\n"));
                    GetAttrStrFromFontStack(&p_fonts, b_bold, b_italic, b_uline,
                                             CFRangeMake(0, 1),
                                             p_attrnode);
                    CFAttributedStringReplaceAttributedString(p_attrString,
                                    CFRangeMake(CFAttributedStringGetLength(p_attrString), 0),
                                    p_attrnode);
                    CFRelease(p_attrnode);
                }
                break;
            case XML_READER_TEXT:
            {
                CFStringRef   p_cfString;
                int           len;
                // Turn any multiple-whitespaces into single spaces
                if (!node)
                    break;
                char *dup = strdup(node);
                if (!dup)
                    break;
                char *s = strpbrk(dup, "\t\r\n ");
                while(s)
                {
                    int i_whitespace = strspn(s, "\t\r\n ");
                    if (i_whitespace > 1)
                        memmove(&s[1],
                                 &s[i_whitespace],
                                 strlen(s) - i_whitespace + 1);
                    *s++ = ' ';
                    s = strpbrk(s, "\t\r\n ");
                }
                CFMutableAttributedStringRef p_attrnode = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
                p_cfString = CFStringCreateWithCString(NULL, dup, kCFStringEncodingUTF8);
                CFAttributedStringReplaceString(p_attrnode, CFRangeMake(0, 0), p_cfString);
                CFRelease(p_cfString);
                len = CFAttributedStringGetLength(p_attrnode);
                GetAttrStrFromFontStack(&p_fonts, b_bold, b_italic, b_uline,
                                         CFRangeMake(0, len),
                                         p_attrnode);
                CFAttributedStringReplaceAttributedString(p_attrString,
                                CFRangeMake(CFAttributedStringGetLength(p_attrString), 0),
                                p_attrnode);
                CFRelease(p_attrnode);
                free(dup);
                break;
            }
        }
    }
    while(VLC_SUCCESS == PopFont(&p_fonts));
    return rv;
}
