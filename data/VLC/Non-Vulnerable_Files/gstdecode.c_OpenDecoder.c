 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = ( decoder_t* )p_this;
    decoder_sys_t *p_sys;
    GstStateChangeReturn i_ret;
    gboolean b_ret;
    sink_src_caps_t caps = { NULL, NULL };
    GstStructure *p_str;
    GstAppSrcCallbacks cb;
    int i_rval = VLC_SUCCESS;
    GList *p_list;
    bool dbin;
#define VLC_GST_CHECK( r, v, s, t ) \
    { if( r == v ){ msg_Err( p_dec, s ); i_rval = t; goto fail; } }
    vlc_gst_init( );
    p_str = vlc_to_gst_fmt( &p_dec->fmt_in );
    if( !p_str )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    p_sys = p_dec->p_sys = calloc( 1, sizeof( *p_sys ) );
    if( p_sys == NULL )
    {
        gst_structure_free( p_str );
        return VLC_ENOMEM;
    }
    dbin = var_CreateGetBool( p_dec, "use-decodebin" );
    msg_Dbg( p_dec, "Using decodebin? %s", dbin ? "yes ":"no" );
    caps.p_sinkcaps = gst_caps_new_empty( );
    gst_caps_append_structure( caps.p_sinkcaps, p_str );
    /* Currently supports only system memory raw output format */
    caps.p_srccaps = gst_caps_new_empty_simple( "video/x-raw" );
    /* Get the list of all the available gstreamer decoders */
    p_list = gst_element_factory_list_get_elements(
            GST_ELEMENT_FACTORY_TYPE_DECODER, GST_RANK_MARGINAL );
    VLC_GST_CHECK( p_list, NULL, "no decoder list found", VLC_ENOMOD );
    if( !dbin )
    {
        GList *p_l;
        /* Sort them as per ranks */
        p_list = g_list_sort( p_list, gst_plugin_feature_rank_compare_func );
        VLC_GST_CHECK( p_list, NULL, "failed to sort decoders list",
                VLC_ENOMOD );
        p_l = g_list_find_custom( p_list, &caps, find_decoder_func );
        VLC_GST_CHECK( p_l, NULL, "no suitable decoder found",
                VLC_ENOMOD );
        /* create the decoder with highest rank */
        p_sys->p_decode_in = gst_element_factory_create(
                ( GstElementFactory* )p_l->data, NULL );
        VLC_GST_CHECK( p_sys->p_decode_in, NULL,
                "failed to create decoder", VLC_ENOMOD );
    }
    else
    {
        GList *p_l;
        /* Just check if any suitable decoder exists, rest will be
         * handled by decodebin */
        p_l = g_list_find_custom( p_list, &caps, find_decoder_func );
        VLC_GST_CHECK( p_l, NULL, "no suitable decoder found",
                VLC_ENOMOD );
    }
    gst_plugin_feature_list_free( p_list );
    p_list = NULL;
    gst_caps_unref( caps.p_srccaps );
    caps.p_srccaps = NULL;
    p_sys->b_prerolled = false;
    p_sys->b_running = false;
    /* Queue: GStreamer thread will dump buffers into this queue,
     * DecodeBlock() will pop out the buffers from the queue */
    p_sys->p_que = gst_atomic_queue_new( 0 );
    VLC_GST_CHECK( p_sys->p_que, NULL, "failed to create queue",
            VLC_ENOMEM );
    p_sys->p_decode_src = gst_element_factory_make( "appsrc", NULL );
    VLC_GST_CHECK( p_sys->p_decode_src, NULL, "appsrc not found",
            VLC_ENOMOD );
    g_object_set( G_OBJECT( p_sys->p_decode_src ), "caps", caps.p_sinkcaps,
            "block", FALSE, "emit-signals", TRUE, "format", GST_FORMAT_BYTES,
            "stream-type", GST_APP_STREAM_TYPE_SEEKABLE,
            /* Making DecodeBlock() to block on appsrc with max queue size of 1 byte.
             * This will make the push_buffer() tightly coupled with the buffer
             * flow from appsrc -> decoder. push_buffer() will only return when
             * the same buffer it just fed to appsrc has also been fed to the
             * decoder element as well */
            "block", TRUE, "max-bytes", ( guint64 )1, NULL );
    gst_caps_unref( caps.p_sinkcaps );
    caps.p_sinkcaps = NULL;
    cb.enough_data = cb.need_data = NULL;
    cb.seek_data = seek_data_cb;
    gst_app_src_set_callbacks( GST_APP_SRC( p_sys->p_decode_src ),
            &cb, p_dec, NULL );
    if( dbin )
    {
        p_sys->p_decode_in = gst_element_factory_make( "decodebin", NULL );
        VLC_GST_CHECK( p_sys->p_decode_in, NULL, "decodebin not found",
                VLC_ENOMOD );
        //g_object_set( G_OBJECT( p_sys->p_decode_in ),
        //"max-size-buffers", 2, NULL );
        g_signal_connect( G_OBJECT( p_sys->p_decode_in ), "pad-added",
                G_CALLBACK( pad_added_cb ), p_dec );
        g_signal_connect( G_OBJECT( p_sys->p_decode_in ), "no-more-pads",
                G_CALLBACK( no_more_pads_cb ), p_dec );
    }
    /* fakesink: will emit signal for every available buffer */
    p_sys->p_decode_out = gst_element_factory_make( "fakesink", NULL );
    VLC_GST_CHECK( p_sys->p_decode_out, NULL, "fakesink not found",
            VLC_ENOMOD );
    /* connect to the signal with the callback */
    g_object_set( G_OBJECT( p_sys->p_decode_out ), "sync", FALSE,
            "enable-last-sample", FALSE, "signal-handoffs", TRUE, NULL );
    g_signal_connect( G_OBJECT( p_sys->p_decode_out ), "handoff",
            G_CALLBACK( frame_handoff_cb ), p_dec );
    p_sys->p_decoder = GST_ELEMENT( gst_bin_new( "decoder" ) );
    VLC_GST_CHECK( p_sys->p_decoder, NULL, "bin not found", VLC_ENOMOD );
    p_sys->p_bus = gst_bus_new( );
    VLC_GST_CHECK( p_sys->p_bus, NULL, "failed to create bus",
            VLC_ENOMOD );
    gst_element_set_bus( p_sys->p_decoder, p_sys->p_bus );
    gst_bin_add_many( GST_BIN( p_sys->p_decoder ),
            p_sys->p_decode_src, p_sys->p_decode_in,
            p_sys->p_decode_out, NULL );
    gst_object_ref( p_sys->p_decode_src );
    gst_object_ref( p_sys->p_decode_in );
    gst_object_ref( p_sys->p_decode_out );
    b_ret = gst_element_link( p_sys->p_decode_src, p_sys->p_decode_in );
    VLC_GST_CHECK( b_ret, FALSE, "failed to link src <-> in",
            VLC_EGENERIC );
    if( !dbin )
    {
        b_ret = gst_element_link( p_sys->p_decode_in, p_sys->p_decode_out );
        VLC_GST_CHECK( b_ret, FALSE, "failed to link in <-> out",
                VLC_EGENERIC );
    }
    p_dec->fmt_out.i_cat = p_dec->fmt_in.i_cat;
    /* set the pipeline to playing */
    i_ret = gst_element_set_state( p_sys->p_decoder, GST_STATE_PLAYING );
    VLC_GST_CHECK( i_ret, GST_STATE_CHANGE_FAILURE,
            "set state failure", VLC_EGENERIC );
    p_sys->b_running = true;
    /* Force packetized for now */
    p_dec->b_need_packetized = true;
    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;
    return VLC_SUCCESS;
fail:
    if( caps.p_sinkcaps )
        gst_caps_unref( caps.p_sinkcaps );
    if( caps.p_srccaps )
        gst_caps_unref( caps.p_srccaps );
    if( p_list )
        gst_plugin_feature_list_free( p_list );
    CloseDecoder( ( vlc_object_t* )p_dec );
    return i_rval;
}
