/* */
static entry_t Lookup( const staticentry_t p_list[], vlc_fourcc_t i_fourcc )
{
    const char *p_class = NULL;
    const char *psz_description = NULL;
    entry_t e = B(0, "");
    for( int i = 0; ; i++ )
    {
        const staticentry_t *p = &p_list[i];
        const vlc_fourcc_t i_entry_fourcc = CreateFourcc( p->p_fourcc );
        const vlc_fourcc_t i_entry_class = CreateFourcc( p->p_class );
        if( i_entry_fourcc == 0 )
            break;
        if( i_entry_class != 0 )
        {
            p_class = p->p_class;
            psz_description = p->psz_description;
        }
        if( i_entry_fourcc == i_fourcc )
        {
            assert( p_class != NULL );
            memcpy( e.p_class, p_class, 4 );
            memcpy( e.p_fourcc, p->p_fourcc, 4 );
            e.psz_description = p->psz_description[0] != '\0' ?
                                p->psz_description : psz_description;
            break;
        }
    }
    return e;
}
