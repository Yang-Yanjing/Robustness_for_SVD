/* Get the number of outputs */
static unsigned CountMonitors( vlc_object_t *obj )
{
    char *psz_display = var_InheritString( obj, "x11-display" );
    int snum;
    xcb_connection_t *conn = xcb_connect( psz_display, &snum );
    free( psz_display );
    if( xcb_connection_has_error( conn ) )
        return 0;
    const xcb_setup_t *setup = xcb_get_setup( conn );
    xcb_screen_t *scr = NULL;
    for( xcb_screen_iterator_t i = xcb_setup_roots_iterator( setup );
         i.rem > 0; xcb_screen_next( &i ) )
    {
         if( snum == 0 )
         {
             scr = i.data;
             break;
         }
         snum--;
    }
    unsigned n = 0;
    if( scr == NULL )
        goto error;
    xcb_randr_query_version_reply_t *v =
        xcb_randr_query_version_reply( conn,
            xcb_randr_query_version( conn, 1, 2 ), NULL );
    if( v == NULL )
        goto error;
    msg_Dbg( obj, "using X RandR extension v%"PRIu32".%"PRIu32,
             v->major_version, v->minor_version );
    free( v );
    xcb_randr_get_screen_resources_reply_t *r =
        xcb_randr_get_screen_resources_reply( conn,
            xcb_randr_get_screen_resources( conn, scr->root ), NULL );
    if( r == NULL )
        goto error;
    const xcb_randr_output_t *outputs =
        xcb_randr_get_screen_resources_outputs( r );
    for( unsigned i = 0; i < r->num_outputs; i++ )
    {
        xcb_randr_get_output_info_reply_t *output =
            xcb_randr_get_output_info_reply( conn,
                xcb_randr_get_output_info( conn, outputs[i], 0 ), NULL );
        if( output == NULL )
            continue;
        /* FIXME: do not count cloned outputs multiple times */
        /* XXX: what to do with UNKNOWN state connections? */
        n += output->connection == XCB_RANDR_CONNECTION_CONNECTED;
        free( output );
    }
    free( r );
    msg_Dbg( obj, "X randr has %u outputs", n );
error:
    xcb_disconnect( conn );
    return n;
}
