}
static void FreeLine( line_desc_t *p_line )
{
    for( int i = 0; i < p_line->i_character_count; i++ )
    {
        line_character_t *ch = &p_line->p_character[i];
        FT_Done_Glyph( (FT_Glyph)ch->p_glyph );
        if( ch->p_outline )
            FT_Done_Glyph( (FT_Glyph)ch->p_outline );
        if( ch->p_shadow )
            FT_Done_Glyph( (FT_Glyph)ch->p_shadow );
    }
    free( p_line->p_character );
    free( p_line );
}
