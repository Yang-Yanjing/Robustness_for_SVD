static int draw_glyphs(DrawTextContext *s, AVFrame *frame,
                       int width, int height,
                       FFDrawColor *color,
                       int x, int y, int borderw)
{
    char *text = s->expanded_text.str;
    uint32_t code = 0;
    int i, x1, y1;
    uint8_t *p;
    Glyph *glyph = NULL;
    for (i = 0, p = text; *p; i++) {
        FT_Bitmap bitmap;
        Glyph dummy = { 0 };
        GET_UTF8(code, *p++, continue;);
        
        if (code == '\n' || code == '\r' || code == '\t')
            continue;
        dummy.code = code;
        glyph = av_tree_find(s->glyphs, &dummy, (void *)glyph_cmp, NULL);
        bitmap = borderw ? glyph->border_bitmap : glyph->bitmap;
        if (glyph->bitmap.pixel_mode != FT_PIXEL_MODE_MONO &&
            glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
            return AVERROR(EINVAL);
        x1 = s->positions[i].x+s->x+x - borderw;
        y1 = s->positions[i].y+s->y+y - borderw;
        ff_blend_mask(&s->dc, color,
                      frame->data, frame->linesize, width, height,
                      bitmap.buffer, bitmap.pitch,
                      bitmap.width, bitmap.rows,
                      bitmap.pixel_mode == FT_PIXEL_MODE_MONO ? 0 : 3,
                      0, x1, y1);
    }
    return 0;
}
