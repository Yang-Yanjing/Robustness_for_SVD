 */
static void LogoListLoad( vlc_object_t *p_this, logo_list_t *p_logo_list,
                          const char *psz_filename )
{
    char *psz_list; /* the list: <logo>[,[<delay>[,[<alpha>]]]][;...] */
    char *psz_original;
    unsigned int i;
    logo_t *p_logo;         /* the parsing's result */
    p_logo_list->i_counter = 0;
    p_logo_list->i_next_pic = 0;
    psz_original = psz_list = strdup( psz_filename );
    if( !psz_list )
        abort();
    /* Count the number logos == number of ';' + 1 */
    p_logo_list->i_count = 1;
    for( i = 0; i < strlen( psz_list ); i++ )
    {
        if( psz_list[i] == ';' )
            p_logo_list->i_count++;
    }
    p_logo_list->p_logo =
    p_logo              = calloc( p_logo_list->i_count, sizeof(*p_logo) );
    if( !p_logo )
        abort();
    /* Fill the data */
    for( i = 0; i < p_logo_list->i_count; i++ )
    {
        char *p_c  = strchr( psz_list, ';' );
        char *p_c2 = strchr( psz_list, ',' );
        p_logo[i].i_alpha = -1; /* use default settings */
        p_logo[i].i_delay = -1; /* use default settings */
        if( p_c2 && ( p_c2 < p_c || !p_c ) )
        {
            /* <logo>,<delay>[,<alpha>] type */
            if( p_c2[1] != ',' && p_c2[1] != ';' && p_c2[1] != '\0' )
                p_logo[i].i_delay = atoi( p_c2+1 );
            *p_c2 = '\0';
            if( ( p_c2 = strchr( p_c2+1, ',' ) )
                && ( p_c2 < p_c || !p_c ) && p_c2[1] != ';' && p_c2[1] != '\0' )
                p_logo[i].i_alpha = atoi( p_c2 + 1 );
        }
        else
        {
            /* <logo> type */
            if( p_c )
                *p_c = '\0';
        }
        msg_Dbg( p_this, "logo file name %s, delay %d, alpha %d",
                 psz_list, p_logo[i].i_delay, p_logo[i].i_alpha );
        p_logo[i].p_pic = LoadImage( p_this, psz_list );
        if( !p_logo[i].p_pic )
        {
            msg_Warn( p_this, "error while loading logo %s, will be skipped",
                      psz_list );
        }
        if( p_c )
            psz_list = &p_c[1];
    }
    /* initialize so that on the first update it will wrap back to 0 */
    p_logo_list->i_counter = p_logo_list->i_count - 1;
    free( psz_original );
}
