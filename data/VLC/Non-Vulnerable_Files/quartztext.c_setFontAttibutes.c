}
static void setFontAttibutes(char *psz_fontname, int i_font_size, uint32_t i_font_color,
        bool b_bold, bool b_italic, bool b_underline,
        CFRange p_range, CFMutableAttributedStringRef p_attrString)
{
    CFStringRef p_cfString;
    CTFontRef   p_font;
    // fallback on default
    if (!psz_fontname)
        psz_fontname = (char *)DEFAULT_FONT;
    p_cfString = CFStringCreateWithCString(kCFAllocatorDefault,
                                            psz_fontname,
                                            kCFStringEncodingUTF8);
    p_font     = CTFontCreateWithName(p_cfString,
                                       (float)i_font_size,
                                       NULL);
    CFRelease(p_cfString);
    CFAttributedStringSetAttribute(p_attrString,
                                    p_range,
                                    kCTFontAttributeName,
                                    p_font);
    CFRelease(p_font);
    // Handle Underline
    SInt32 _uline;
    if (b_underline)
        _uline = kCTUnderlineStyleSingle;
    else
        _uline = kCTUnderlineStyleNone;
    CFNumberRef underline = CFNumberCreate(NULL, kCFNumberSInt32Type, &_uline);
    CFAttributedStringSetAttribute(p_attrString,
                                    p_range,
                                    kCTUnderlineStyleAttributeName,
                                    underline);
    CFRelease(underline);
    // Handle Bold
    float _weight;
    if (b_bold)
        _weight = 0.5;
    else
        _weight = 0.0;
    CFNumberRef weight = CFNumberCreate(NULL, kCFNumberFloatType, &_weight);
    CFAttributedStringSetAttribute(p_attrString,
                                    p_range,
                                    kCTFontWeightTrait,
                                    weight);
    CFRelease(weight);
    // Handle Italic
    float _slant;
    if (b_italic)
        _slant = 1.0;
    else
        _slant = 0.0;
    CFNumberRef slant = CFNumberCreate(NULL, kCFNumberFloatType, &_slant);
    CFAttributedStringSetAttribute(p_attrString,
                                    p_range,
                                    kCTFontSlantTrait,
                                    slant);
    CFRelease(slant);
    // fetch invalid colors
    if (i_font_color == 0xFFFFFFFF)
        i_font_color = 0x00FFFFFF;
    // Handle foreground color
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGFloat components[] = { (float)((i_font_color & 0x00ff0000) >> 16) / 255.0,
                             (float)((i_font_color & 0x0000ff00) >>  8) / 255.0,
                             (float)((i_font_color & 0x000000ff)) / 255.0,
                             (float)(255-((i_font_color & 0xff000000) >> 24)) / 255.0 };
    CGColorRef fg_text = CGColorCreate(rgbColorSpace, components);
    CGColorSpaceRelease(rgbColorSpace);
    CFAttributedStringSetAttribute(p_attrString,
                                    p_range,
                                    kCTForegroundColorAttributeName,
                                    fg_text);
    CFRelease(fg_text);
}
