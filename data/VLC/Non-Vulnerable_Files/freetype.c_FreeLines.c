}
static void FreeLines( line_desc_t *p_lines )
{
    for( line_desc_t *p_line = p_lines; p_line != NULL; )
    {
        line_desc_t *p_next = p_line->p_next;
        FreeLine( p_line );
        p_line = p_next;
    }
}
