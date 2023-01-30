 *****************************************************************************/
static void ParseKateComments( decoder_t *p_dec )
{
    char *psz_name, *psz_value, *psz_comment;
    int i = 0;
    while ( i < p_dec->p_sys->kc.comments )
    {
        psz_comment = strdup( p_dec->p_sys->kc.user_comments[i] );
        if( !psz_comment )
            break;
        psz_name = psz_comment;
        psz_value = strchr( psz_comment, '=' );
        if( psz_value )
        {
            *psz_value = '\0';
            psz_value++;
            if( !p_dec->p_description )
                p_dec->p_description = vlc_meta_New();
            if( p_dec->p_description )
                vlc_meta_AddExtra( p_dec->p_description, psz_name, psz_value );
        }
        free( psz_comment );
        i++;
    }
}
