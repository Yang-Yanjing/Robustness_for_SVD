vlc_module_end ()
static int Start( audio_output_t *p_aout, audio_sample_format_t *restrict fmt )
{
    char psz_name[32];
    struct aout_sys_t *p_sys = p_aout->sys;
    int status = VLC_SUCCESS;
    unsigned int i;
    int i_error;
    p_sys->latency = 0;
    p_sys->paused = VLC_TS_INVALID;
    /* Connect to the JACK server */
    snprintf( psz_name, sizeof(psz_name), "vlc_%d", getpid());
    psz_name[sizeof(psz_name) - 1] = '\0';
    p_sys->p_jack_client = jack_client_open( psz_name,
                                             JackNullOption | JackNoStartServer,
                                             NULL );
    if( p_sys->p_jack_client == NULL )
    {
        msg_Err( p_aout, "failed to connect to JACK server" );
        status = VLC_EGENERIC;
        goto error_out;
    }
    /* Set the process callback */
    jack_set_process_callback( p_sys->p_jack_client, Process, p_aout );
    jack_set_graph_order_callback ( p_sys->p_jack_client, GraphChange, p_aout );
    /* JACK only supports fl32 format */
    fmt->i_format = VLC_CODEC_FL32;
    // TODO add buffer size callback
    p_sys->i_rate = fmt->i_rate = jack_get_sample_rate( p_sys->p_jack_client );
    p_aout->play = Play;
    p_aout->pause = Pause;
    p_aout->flush = Flush;
    p_aout->time_get = TimeGet;
    aout_SoftVolumeStart( p_aout );
    p_sys->i_channels = aout_FormatNbChannels( fmt );
    aout_FormatPrepare(fmt);
    p_sys->p_jack_ports = malloc( p_sys->i_channels *
                                  sizeof(jack_port_t *) );
    if( p_sys->p_jack_ports == NULL )
    {
        status = VLC_ENOMEM;
        goto error_out;
    }
    p_sys->p_jack_buffers = malloc( p_sys->i_channels *
                                    sizeof(jack_sample_t *) );
    if( p_sys->p_jack_buffers == NULL )
    {
        status = VLC_ENOMEM;
        goto error_out;
    }
    const size_t buf_sz = AOUT_MAX_ADVANCE_TIME * fmt->i_rate *
        fmt->i_bytes_per_frame / CLOCK_FREQ;
    p_sys->p_jack_ringbuffer = jack_ringbuffer_create( buf_sz );
    if( p_sys->p_jack_ringbuffer == NULL )
    {
        status = VLC_ENOMEM;
        goto error_out;
    }
    if( jack_ringbuffer_mlock( p_sys->p_jack_ringbuffer ))
    {
        msg_Warn( p_aout, "failed to lock JACK ringbuffer in memory" );
    }
    /* Create the output ports */
    for( i = 0; i < p_sys->i_channels; i++ )
    {
        snprintf( psz_name, sizeof(psz_name), "out_%d", i + 1);
        psz_name[sizeof(psz_name) - 1] = '\0';
        p_sys->p_jack_ports[i] = jack_port_register( p_sys->p_jack_client,
                psz_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0 );
        if( p_sys->p_jack_ports[i] == NULL )
        {
            msg_Err( p_aout, "failed to register a JACK port" );
            status = VLC_EGENERIC;
            goto error_out;
        }
    }
    /* Tell the JACK server we are ready */
    i_error = jack_activate( p_sys->p_jack_client );
    if( i_error )
    {
        msg_Err( p_aout, "failed to activate JACK client (error %d)", i_error );
        status = VLC_EGENERIC;
        goto error_out;
    }
    /* Auto connect ports if we were asked to */
    if( var_InheritBool( p_aout, AUTO_CONNECT_OPTION ) )
    {
        unsigned int i_in_ports;
        char *psz_regex = var_InheritString( p_aout, CONNECT_REGEX_OPTION );
        const char **pp_in_ports = jack_get_ports( p_sys->p_jack_client,
                                                   psz_regex, NULL,
                                                   JackPortIsInput );
        free( psz_regex );
        /* Count the number of returned ports */
        i_in_ports = 0;
        while( pp_in_ports && pp_in_ports[i_in_ports] )
        {
            i_in_ports++;
        }
        /* Tie the output ports to JACK input ports */
        for( i = 0; i_in_ports > 0 && i < p_sys->i_channels; i++ )
        {
            const char* psz_in = pp_in_ports[i % i_in_ports];
            const char* psz_out = jack_port_name( p_sys->p_jack_ports[i] );
            i_error = jack_connect( p_sys->p_jack_client, psz_out, psz_in );
            if( i_error )
            {
                msg_Err( p_aout, "failed to connect port %s to port %s (error %d)",
                         psz_out, psz_in, i_error );
            }
            else
            {
                msg_Dbg( p_aout, "connecting port %s to port %s",
                         psz_out, psz_in );
            }
        }
        free( pp_in_ports );
    }
    msg_Dbg( p_aout, "JACK audio output initialized (%d channels, rate=%d)",
             p_sys->i_channels, fmt->i_rate );
error_out:
    /* Clean up, if an error occurred */
    if( status != VLC_SUCCESS )
    {
        if( p_sys->p_jack_client )
        {
            jack_deactivate( p_sys->p_jack_client );
            jack_client_close( p_sys->p_jack_client );
        }
        if( p_sys->p_jack_ringbuffer )
            jack_ringbuffer_free( p_sys->p_jack_ringbuffer );
        free( p_sys->p_jack_ports );
        free( p_sys->p_jack_buffers );
    }
    return status;
}
