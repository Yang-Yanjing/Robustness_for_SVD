static void load_freetype_font(AVFilterContext *ctx)
{
    static const char str[] = "EF G A BC D ";
    ShowCQTContext *s = ctx->priv;
    FT_Library lib = NULL;
    FT_Face face = NULL;
    int video_scale = s->fullhd ? 2 : 1;
    int video_width = (VIDEO_WIDTH/2) * video_scale;
    int font_height = (FONT_HEIGHT/2) * video_scale;
    int font_width = 8 * video_scale;
    int font_repeat = font_width * 12;
    int linear_hori_advance = font_width * 65536;
    int non_monospace_warning = 0;
    int x;
    s->font_alpha = NULL;
    if (!s->fontfile)
        return;
    if (FT_Init_FreeType(&lib))
        goto fail;
    if (FT_New_Face(lib, s->fontfile, 0, &face))
        goto fail;
    if (FT_Set_Char_Size(face, 16*64, 0, 0, 0))
        goto fail;
    if (FT_Load_Char(face, 'A', FT_LOAD_RENDER))
        goto fail;
    if (FT_Set_Char_Size(face, 16*64 * linear_hori_advance / face->glyph->linearHoriAdvance, 0, 0, 0))
        goto fail;
    s->font_alpha = av_malloc_array(font_height, video_width);
    if (!s->font_alpha)
        goto fail;
    memset(s->font_alpha, 0, font_height * video_width);
    for (x = 0; x < 12; x++) {
        int sx, sy, rx, bx, by, dx, dy;
        if (str[x] == ' ')
            continue;
        if (FT_Load_Char(face, str[x], FT_LOAD_RENDER))
            goto fail;
        if (face->glyph->advance.x != font_width*64 && !non_monospace_warning) {
            av_log(ctx, AV_LOG_WARNING, "Font is not monospace\n");
            non_monospace_warning = 1;
        }
        sy = font_height - 4*video_scale - face->glyph->bitmap_top;
        for (rx = 0; rx < 10; rx++) {
            sx = rx * font_repeat + x * font_width + face->glyph->bitmap_left;
            for (by = 0; by < face->glyph->bitmap.rows; by++) {
                dy = by + sy;
                if (dy < 0)
                    continue;
                if (dy >= font_height)
                    break;
                for (bx = 0; bx < face->glyph->bitmap.width; bx++) {
                    dx = bx + sx;
                    if (dx < 0)
                        continue;
                    if (dx >= video_width)
                        break;
                    s->font_alpha[dy*video_width+dx] = face->glyph->bitmap.buffer[by*face->glyph->bitmap.width+bx];
                }
            }
        }
    }
    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    return;
    fail:
    av_log(ctx, AV_LOG_WARNING, "Error while loading freetype font, using default font instead\n");
    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    av_freep(&s->font_alpha);
    return;
}
