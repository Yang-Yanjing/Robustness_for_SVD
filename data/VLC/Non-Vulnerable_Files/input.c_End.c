 *****************************************************************************/
static void End( input_thread_t * p_input )
{
    int i;
    /* We are at the end */
    input_ChangeState( p_input, END_S );
    /* Clean control variables */
    input_ControlVarStop( p_input );
    /* Stop es out activity */
    es_out_SetMode( p_input->p->p_es_out, ES_OUT_MODE_NONE );
    /* Clean up master */
    InputSourceClean( &p_input->p->input );
    /* Delete slave */
    for( i = 0; i < p_input->p->i_slave; i++ )
    {
        InputSourceClean( p_input->p->slave[i] );
        free( p_input->p->slave[i] );
    }
    free( p_input->p->slave );
    /* Unload all modules */
    if( p_input->p->p_es_out )
        es_out_Delete( p_input->p->p_es_out );
    es_out_SetMode( p_input->p->p_es_out_display, ES_OUT_MODE_END );
    if( !p_input->b_preparsing )
    {
#define CL_CO( c ) stats_CounterClean( p_input->p->counters.p_##c ); p_input->p->counters.p_##c = NULL;
        if( libvlc_stats( p_input ) )
        {
            /* make sure we are up to date */
            stats_ComputeInputStats( p_input, p_input->p->p_item->p_stats );
            CL_CO( read_bytes );
            CL_CO( read_packets );
            CL_CO( demux_read );
            CL_CO( input_bitrate );
            CL_CO( demux_bitrate );
            CL_CO( demux_corrupted );
            CL_CO( demux_discontinuity );
            CL_CO( played_abuffers );
            CL_CO( lost_abuffers );
            CL_CO( displayed_pictures );
            CL_CO( lost_pictures );
            CL_CO( decoded_audio) ;
            CL_CO( decoded_video );
            CL_CO( decoded_sub) ;
        }
        /* Close optional stream output instance */
        if( p_input->p->p_sout )
        {
            CL_CO( sout_sent_packets );
            CL_CO( sout_sent_bytes );
            CL_CO( sout_send_bitrate );
        }
#undef CL_CO
    }
    vlc_mutex_lock( &p_input->p->p_item->lock );
    if( p_input->p->i_attachment > 0 )
    {
        for( i = 0; i < p_input->p->i_attachment; i++ )
            vlc_input_attachment_Delete( p_input->p->attachment[i] );
        TAB_CLEAN( p_input->p->i_attachment, p_input->p->attachment );
        free( p_input->p->attachment_demux);
        p_input->p->attachment_demux = NULL;
    }
    vlc_mutex_unlock( &p_input->p->p_item->lock );
    /* */
    input_resource_RequestSout( p_input->p->p_resource,
                                 p_input->p->p_sout, NULL );
    input_resource_SetInput( p_input->p->p_resource, NULL );
    if( p_input->p->p_resource_private )
        input_resource_Terminate( p_input->p->p_resource_private );
}
