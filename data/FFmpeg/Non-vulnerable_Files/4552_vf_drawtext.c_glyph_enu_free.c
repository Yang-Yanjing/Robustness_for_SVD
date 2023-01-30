static int glyph_enu_free(void *opaque, void *elem)
{
    Glyph *glyph = elem;
    FT_Done_Glyph(glyph->glyph);
    FT_Done_Glyph(glyph->border_glyph);
    av_free(elem);
    return 0;
}
