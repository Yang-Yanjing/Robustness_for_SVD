}
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    char *psz_line;
    char *psz_attrvalue;
    char *psz_version = NULL;
    char *psz_url = NULL;
    char *psz_docid = NULL;
    int i_duration = -1;
    char *psz_title = NULL;
    char *psz_description = NULL;
    input_item_t *p_input;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    input_item_node_t *p_subitems = input_item_node_Create( p_current_input );
    p_sys->p_current_input = p_current_input;
    while( ( psz_line = stream_ReadLine( p_demux->s ) ) )
    {
        if( *psz_line == '#' )
        {
            /* This is a comment */
            free( psz_line );
            continue;
        }
        psz_attrvalue = strchr( psz_line, ':' );
        if( !psz_attrvalue )
        {
            msg_Dbg( p_demux, "Unable to parse line (%s)", psz_line );
            free( psz_line );
            continue;
        }
        *psz_attrvalue = '\0';
        psz_attrvalue++;
        if( !strcmp( psz_line, "gvp_version" ) )
        {
            psz_version = strdup( psz_attrvalue );
        }
        else if( !strcmp( psz_line, "url" ) )
        {
            psz_url = strdup( psz_attrvalue );
        }
        else if( !strcmp( psz_line, "docid" ) )
        {
            psz_docid = strdup( psz_attrvalue );
        }
        else if( !strcmp( psz_line, "duration" ) )
        {
            i_duration = atoi( psz_attrvalue );
        }
        else if( !strcmp( psz_line, "title" ) )
        {
            psz_title = strdup( psz_attrvalue );
        }
        else if( !strcmp( psz_line, "description" ) )
        {
            char *buf;
            if( !psz_description )
            {
                psz_description = strdup( psz_attrvalue );
            }
            else
            {
                /* handle multi-line descriptions */
                if( asprintf( &buf, "%s\n%s", psz_description, psz_attrvalue ) == -1 )
                    buf = NULL;
                free( psz_description );
                psz_description = buf;
            }
            /* remove ^M char at the end of the line (if any) */
            buf = psz_description + strlen( psz_description );
            if( buf != psz_description )
            {
                buf--;
                if( *buf == '\r' ) *buf = '\0';
            }
        }
        free( psz_line );
    }
    if( !psz_url )
    {
        msg_Err( p_demux, "URL not found" );
    }
    else
    {
        p_input = input_item_New( psz_url, psz_title );
#define SADD_INFO( type, field ) if( field ) { input_item_AddInfo( \
                    p_input, _("Google Video"), type, "%s", field ) ; }
        SADD_INFO( "gvp_version", psz_version );
        SADD_INFO( "docid", psz_docid );
        SADD_INFO( "description", psz_description );
        input_item_node_AppendItem( p_subitems, p_input );
        vlc_gc_decref( p_input );
    }
    input_item_node_PostAndDelete( p_subitems );
    vlc_gc_decref(p_current_input);
    free( psz_version );
    free( psz_url );
    free( psz_docid );
    free( psz_title );
    free( psz_description );
    return 0; /* Needed for correct operation of go back */
}
