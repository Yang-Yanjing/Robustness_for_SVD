 **/
static int DemuxStation( demux_t *p_demux, xml_reader_t *p_xml_reader,
                         input_item_node_t *p_input_node, bool b_adult )
{
    char *psz_base = NULL; /* */
    char *psz_name = NULL; /* genre name */
    char *psz_mt = NULL; /* mime type */
    char *psz_id = NULL; /* id */
    char *psz_br = NULL; /* bit rate */
    char *psz_genre = NULL; /* genre */
    char *psz_ct = NULL; /* current track */
    char *psz_lc = NULL; /* listener count */
    /* If these are set then it's *not* a radio but a TV */
    char *psz_rt = NULL; /* rating for shoutcast TV */
    char *psz_load = NULL; /* load for shoutcast TV */
    const char *node; /* tag name */
    int i_type;
    while( (i_type = xml_ReaderNextNode( p_xml_reader, &node )) > 0 )
    {
        switch( i_type )
        {
            case XML_READER_STARTELEM:
                // Read the attributes
                if( !strcmp( node, "tunein" ) )
                {
                    const char *name, *value;
                    while( (name = xml_ReaderNextAttr( p_xml_reader, &value )) )
                    {
                        if( !strcmp( name, "base" ) )
                        {
                            free( psz_base );
                            psz_base = strdup( value );
                        }
                        else
                            msg_Warn( p_demux,
                                      "unexpected attribute %s in <%s>",
                                      name, node );
                    }
                }
                else if( !strcmp( node, "station" ) )
                {
                    const char *name, *value;
                    while( (name = xml_ReaderNextAttr( p_xml_reader, &value )) )
                    {
                        char **p = NULL;
                        if( !strcmp( name, "name" ) )
                            p = &psz_name;
                        else if ( !strcmp( name, "mt" ) )
                            p = &psz_mt;
                        else if ( !strcmp( name, "id" ) )
                            p = &psz_id;
                        else if ( !strcmp( name, "br" ) )
                            p = &psz_br;
                        else if ( !strcmp( name, "genre" ) )
                            p = &psz_genre;
                        else if ( !strcmp( name, "ct" ) )
                            p = &psz_ct;
                        else if ( !strcmp( name, "lc" ) )
                            p = &psz_lc;
                        else if ( !strcmp( name, "rt" ) )
                            p = &psz_rt;
                        else if ( !strcmp( name, "load" ) )
                            p = &psz_load;
                        if( p != NULL )
                        {
                            free( *p );
                            *p = strdup( value );
                        }
                        else
                            msg_Warn( p_demux,
                                      "unexpected attribute %s in <%s>",
                                      name, node );
                    }
                }
                break;
            // End element
            case XML_READER_ENDELEM:
                if( !strcmp( node, "station" ) &&
                    ( psz_base || ( psz_rt && psz_load &&
                    ( b_adult || strcmp( psz_rt, "NC17" ) ) ) ) )
                {
                    char *psz_mrl = NULL;
                    if( psz_rt || psz_load )
                    {
                        /* tv */
                        if( asprintf( &psz_mrl, SHOUTCAST_TV_TUNEIN_URL "%s",
                                 psz_id ) == -1)
                            psz_mrl = NULL;
                    }
                    else
                    {
                        /* radio */
                        if( asprintf( &psz_mrl, SHOUTCAST_TUNEIN_BASE_URL "%s?id=%s",
                             psz_base, psz_id ) == -1 )
                            psz_mrl = NULL;
                    }
                    /* Create the item */
                    input_item_t *p_input = NULL;
                    if( likely(psz_mrl != NULL) )
                    {
                        resolve_xml_special_chars( psz_mrl );
                        p_input = input_item_New( psz_mrl, psz_name );
                        free( psz_mrl );
                    }
                    if( likely(p_input != NULL) )
                    {
                        input_item_CopyOptions( p_input_node->p_item, p_input );
#define SADD_INFO( type, field ) \
                    if( field ) \
                        input_item_AddInfo( p_input, _("Shoutcast"), \
                                            vlc_gettext(type), "%s", field )
                        SADD_INFO( N_("Mime"), psz_mt );
                        SADD_INFO( N_("Bitrate"), psz_br );
                        SADD_INFO( N_("Listeners"), psz_lc );
                        SADD_INFO( N_("Load"), psz_load );
                        if( psz_genre )
                            input_item_SetGenre( p_input, psz_genre );
                        if( psz_ct )
                            input_item_SetNowPlaying( p_input, psz_ct );
                        if( psz_rt )
                            input_item_SetRating( p_input, psz_rt );
                        input_item_node_AppendItem( p_input_node, p_input );
                        vlc_gc_decref( p_input );
                    }
                    FREENULL( psz_base );
                    FREENULL( psz_name );
                    FREENULL( psz_mt );
                    FREENULL( psz_id );
                    FREENULL( psz_br );
                    FREENULL( psz_genre );
                    FREENULL( psz_ct );
                    FREENULL( psz_lc );
                    FREENULL( psz_rt );
                    FREENULL( psz_load );
                }
                break;
        }
    }
    /* FIXME: leaks on missing ENDELEMENT? */
    return 0;
}
