static void drawtext(AVFrame *pic, int x, int y, int ftid, const uint8_t *color, const char *fmt, ...)
{
    int i;
    char buf[128] = {0};
    const uint8_t *font;
    int font_height;
    va_list vl;
    if      (ftid == FONT16) font = avpriv_vga16_font, font_height = 16;
    else if (ftid == FONT8)  font = avpriv_cga_font,   font_height =  8;
    else return;
    va_start(vl, fmt);
    vsnprintf(buf, sizeof(buf), fmt, vl);
    va_end(vl);
    for (i = 0; buf[i]; i++) {
        int char_y, mask;
        uint8_t *p = pic->data[0] + y*pic->linesize[0] + (x + i*8)*3;
        for (char_y = 0; char_y < font_height; char_y++) {
            for (mask = 0x80; mask; mask >>= 1) {
                if (font[buf[i] * font_height + char_y] & mask)
                    memcpy(p, color, 3);
                else
                    memcpy(p, "\x00\x00\x00", 3);
                p += 3;
            }
            p += pic->linesize[0] - 8*3;
        }
    }
}
