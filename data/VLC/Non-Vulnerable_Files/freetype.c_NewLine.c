}
static line_desc_t *NewLine( int i_count )
{
    line_desc_t *p_line = malloc( sizeof(*p_line) );
    if( !p_line )
        return NULL;
    p_line->p_next = NULL;
    p_line->i_width = 0;
    p_line->i_base_line = 0;
    p_line->i_character_count = 0;
    p_line->p_character = calloc( i_count, sizeof(*p_line->p_character) );
    if( !p_line->p_character )
    {
        free( p_line );
        return NULL;
    }
    return p_line;
}
