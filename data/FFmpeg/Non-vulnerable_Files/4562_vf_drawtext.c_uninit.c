static av_cold void uninit(AVFilterContext *ctx)
{
    DrawTextContext *s = ctx->priv;
    av_expr_free(s->x_pexpr);
    av_expr_free(s->y_pexpr);
    s->x_pexpr = s->y_pexpr = NULL;
    av_freep(&s->positions);
    s->nb_positions = 0;
    av_tree_enumerate(s->glyphs, NULL, NULL, glyph_enu_free);
    av_tree_destroy(s->glyphs);
    s->glyphs = NULL;
    FT_Done_Face(s->face);
    FT_Stroker_Done(s->stroker);
    FT_Done_FreeType(s->library);
    av_bprint_finalize(&s->expanded_text, NULL);
    av_bprint_finalize(&s->expanded_fontcolor, NULL);
}
