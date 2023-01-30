}
static void FixGlyph( FT_Glyph glyph, FT_BBox *p_bbox, FT_Face face, const FT_Vector *p_pen )
{
    FT_BitmapGlyph glyph_bmp = (FT_BitmapGlyph)glyph;
    if( p_bbox->xMin >= p_bbox->xMax )
    {
        p_bbox->xMin = FT_CEIL(p_pen->x);
        p_bbox->xMax = FT_CEIL(p_pen->x + face->glyph->advance.x);
        glyph_bmp->left = p_bbox->xMin;
    }
    if( p_bbox->yMin >= p_bbox->yMax )
    {
        p_bbox->yMax = FT_CEIL(p_pen->y);
        p_bbox->yMin = FT_CEIL(p_pen->y + face->glyph->advance.y);
        glyph_bmp->top  = p_bbox->yMax;
    }
}
