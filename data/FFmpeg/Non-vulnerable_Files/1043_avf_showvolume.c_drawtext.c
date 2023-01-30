static void drawtext(AVFrame *pic, int x, int y, const char *txt)
{
    const uint8_t *font;
    int font_height;
    int i;
    font = avpriv_cga_font,   font_height =  8;
    for (i = 0; txt[i]; i++) {
        int char_y, mask;
        uint8_t *p = pic->data[0] + y*pic->linesize[0] + (x + i*8)*4;
        for (char_y = 0; char_y < font_height; char_y++) {
            for (mask = 0x80; mask; mask >>= 1) {
                if (font[txt[i] * font_height + char_y] & mask)
                    AV_WN32(p, ~AV_RN32(p));
                p += 4;
            }
            p += pic->linesize[0] - 8*4;
        }
    }
}
