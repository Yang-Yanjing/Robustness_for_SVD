 *****************************************************************************/
static input_thread_t *Create( vlc_object_t *p_parent, input_item_t *p_item,
                               const char *psz_header, bool b_quick,
                               input_resource_t *p_resource )
{
    input_thread_t *p_input = NULL;                 /* thread descriptor */
    int i;
    /* Allocate descriptor */
    p_input = vlc_custom_create( p_parent, sizeof( *p_input ), "input" );
    if( p_input == NULL )
        return NULL;
    /* Construct a nice name for the input timer */
    char psz_timer_name[255];
    char * psz_name = input_item_GetName( p_item );
    snprintf( psz_timer_name, sizeof(psz_timer_name),
              "input launching for '%s'", psz_name );
    msg_Dbg( p_input, "Creating an input for '%s'", psz_name);
    free( psz_name );
    p_input->p = calloc( 1, sizeof( input_thread_private_t ) );
    if( !p_input->p )
    {
        vlc_object_release( p_input );
        return NULL;
    }
    /* Parse input options */
    vlc_mutex_lock( &p_item->lock );
    assert( (int)p_item->optflagc == p_item->i_options );
    for( i = 0; i < p_item->i_options; i++ )
        var_OptionParse( VLC_OBJECT(p_input), p_item->ppsz_options[i],
                         !!(p_item->optflagv[i] & VLC_INPUT_OPTION_TRUSTED) );
    vlc_mutex_unlock( &p_item->lock );
    p_input->b_preparsing = b_quick;
    p_input->psz_header = psz_header ? strdup( psz_header ) : NULL;
    /* Init Common fields */
    p_input->b_eof = false;
    p_input->p->b_can_pace_control = true;
    p_input->p->i_start = 0;
    p_input->p->i_time  = 0;
    p_input->p->i_stop  = 0;
    p_input->p->i_run   = 0;
    p_input->p->i_title = 0;
    p_input->p->title = NULL;
    p_input->p->i_title_offset = p_input->p->i_seekpoint_offset = 0;
    p_input->p->i_state = INIT_S;
    p_input->p->i_rate = INPUT_RATE_DEFAULT;
    p_input->p->b_recording = false;
    memset( &p_input->p->bookmark, 0, sizeof(p_input->p->bookmark) );
    TAB_INIT( p_input->p->i_bookmark, p_input->p->pp_bookmark );
    TAB_INIT( p_input->p->i_attachment, p_input->p->attachment );
    p_input->p->attachment_demux = NULL;
    p_input->p->p_sout   = NULL;
    p_input->p->b_out_pace_control = false;
    vlc_gc_incref( p_item ); /* Released in Destructor() */
    p_input->p->p_item = p_item;
    /* Init Input fields */
    p_input->p->input.p_demux  = NULL;
    p_input->p->input.b_title_demux = false;
    p_input->p->input.i_title  = 0;
    p_input->p->input.title    = NULL;
    p_input->p->input.i_title_offset = p_input->p->input.i_seekpoint_offset = 0;
    p_input->p->input.b_can_pace_control = true;
    p_input->p->input.b_can_rate_control = true;
    p_input->p->input.b_rescale_ts = true;
    p_input->p->input.b_eof = false;
    vlc_mutex_lock( &p_item->lock );
    if( !p_item->p_stats )
        p_item->p_stats = stats_NewInputStats( p_input );
    vlc_mutex_unlock( &p_item->lock );
    /* No slave */
    p_input->p->i_slave = 0;
    p_input->p->slave   = NULL;
    /* */
    if( p_resource )
    {
        p_input->p->p_resource_private = NULL;
        p_input->p->p_resource = input_resource_Hold( p_resource );
    }
    else
    {
        p_input->p->p_resource_private = input_resource_New( VLC_OBJECT( p_input ) );
        p_input->p->p_resource = input_resource_Hold( p_input->p->p_resource_private );
    }
    input_resource_SetInput( p_input->p->p_resource, p_input );
    /* Init control buffer */
    vlc_mutex_init( &p_input->p->lock_control );
    vlc_cond_init( &p_input->p->wait_control );
    p_input->p->i_control = 0;
    p_input->p->b_abort = false;
    p_input->p->is_running = false;
    /* Create Object Variables for private use only */
    input_ConfigVarInit( p_input );
    /* Create Objects variables for public Get and Set */
    input_ControlVarInit( p_input );
    /* */
    if( !p_input->b_preparsing )
    {
        char *psz_bookmarks = var_GetNonEmptyString( p_input, "bookmarks" );
        if( psz_bookmarks )
        {
            /* FIXME: have a common cfg parsing routine used by sout and others */
            char *psz_parser, *psz_start, *psz_end;
            psz_parser = psz_bookmarks;
            while( (psz_start = strchr( psz_parser, '{' ) ) )
            {
                 seekpoint_t *p_seekpoint;
                 char backup;
                 psz_start++;
                 psz_end = strchr( psz_start, '}' );
                 if( !psz_end ) break;
                 psz_parser = psz_end + 1;
                 backup = *psz_parser;
                 *psz_parser = 0;
                 *psz_end = ',';
                 p_seekpoint = vlc_seekpoint_New();
                 while( (psz_end = strchr( psz_start, ',' ) ) )
                 {
                     *psz_end = 0;
                     if( !strncmp( psz_start, "name=", 5 ) )
                     {
                         p_seekpoint->psz_name = strdup(psz_start + 5);
                     }
                     else if( !strncmp( psz_start, "bytes=", 6 ) )
                     {
                         p_seekpoint->i_byte_offset = atoll(psz_start + 6);
                     }
                     else if( !strncmp( psz_start, "time=", 5 ) )
                     {
                         p_seekpoint->i_time_offset = atoll(psz_start + 5) *
                                                        1000000;
                     }
                     psz_start = psz_end + 1;
                }
                msg_Dbg( p_input, "adding bookmark: %s, bytes=%"PRId64", time=%"PRId64,
                                  p_seekpoint->psz_name, p_seekpoint->i_byte_offset,
                                  p_seekpoint->i_time_offset );
                input_Control( p_input, INPUT_ADD_BOOKMARK, p_seekpoint );
                vlc_seekpoint_Delete( p_seekpoint );
                *psz_parser = backup;
            }
            free( psz_bookmarks );
        }
    }
    /* Remove 'Now playing' info as it is probably outdated */
    input_item_SetNowPlaying( p_item, NULL );
    input_item_SetESNowPlaying( p_item, NULL );
    input_SendEventMeta( p_input );
    /* */
    if( p_input->b_preparsing )
        p_input->i_flags |= OBJECT_FLAGS_QUIET | OBJECT_FLAGS_NOINTERACT;
    /* Make sure the interaction option is honored */
    if( !var_InheritBool( p_input, "interact" ) )
        p_input->i_flags |= OBJECT_FLAGS_NOINTERACT;
    /* */
    memset( &p_input->p->counters, 0, sizeof( p_input->p->counters ) );
    vlc_mutex_init( &p_input->p->counters.counters_lock );
    p_input->p->p_es_out_display = input_EsOutNew( p_input, p_input->p->i_rate );
    p_input->p->p_es_out = NULL;
    /* Set the destructor when we are sure we are initialized */
    vlc_object_set_destructor( p_input, (vlc_destructor_t)Destructor );
    return p_input;
}
