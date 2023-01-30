 * In case p_aout is NULL, we will use configuration instead of variable */
bool aout_ChangeFilterString( vlc_object_t *p_obj, vlc_object_t *p_aout,
                              const char *psz_variable,
                              const char *psz_name, bool b_add )
{
    if( *psz_name == '\0' )
        return false;
    char *psz_list;
    if( p_aout )
    {
        psz_list = var_GetString( p_aout, psz_variable );
    }
    else
    {
        psz_list = var_CreateGetString( p_obj->p_libvlc, psz_variable );
        var_Destroy( p_obj->p_libvlc, psz_variable );
    }
    /* Split the string into an array of filters */
    int i_count = 1;
    for( char *p = psz_list; p && *p; p++ )
        i_count += *p == ':';
    i_count += b_add;
    const char **ppsz_filter = calloc( i_count, sizeof(*ppsz_filter) );
    if( !ppsz_filter )
    {
        free( psz_list );
        return false;
    }
    bool b_present = false;
    i_count = 0;
    for( char *p = psz_list; p && *p; )
    {
        char *psz_end = strchr(p, ':');
        if( psz_end )
            *psz_end++ = '\0';
        else
            psz_end = p + strlen(p);
        if( *p )
        {
            b_present |= !strcmp( p, psz_name );
            ppsz_filter[i_count++] = p;
        }
        p = psz_end;
    }
    if( b_present == b_add )
    {
        free( ppsz_filter );
        free( psz_list );
        return false;
    }
    if( b_add )
    {
        int i_order = FilterOrder( psz_name );
        int i;
        for( i = 0; i < i_count; i++ )
        {
            if( FilterOrder( ppsz_filter[i] ) > i_order )
                break;
        }
        if( i < i_count )
            memmove( &ppsz_filter[i+1], &ppsz_filter[i], (i_count - i) * sizeof(*ppsz_filter) );
        ppsz_filter[i] = psz_name;
        i_count++;
    }
    else
    {
        for( int i = 0; i < i_count; i++ )
        {
            if( !strcmp( ppsz_filter[i], psz_name ) )
                ppsz_filter[i] = "";
        }
    }
    size_t i_length = 0;
    for( int i = 0; i < i_count; i++ )
        i_length += 1 + strlen( ppsz_filter[i] );
    char *psz_new = malloc( i_length + 1 );
    *psz_new = '\0';
    for( int i = 0; i < i_count; i++ )
    {
        if( *ppsz_filter[i] == '\0' )
            continue;
        if( *psz_new )
            strcat( psz_new, ":" );
        strcat( psz_new, ppsz_filter[i] );
    }
    free( ppsz_filter );
    free( psz_list );
    if( p_aout )
        var_SetString( p_aout, psz_variable, psz_new );
    else
        config_PutPsz( p_obj, psz_variable, psz_new );
    free( psz_new );
    return true;
}
