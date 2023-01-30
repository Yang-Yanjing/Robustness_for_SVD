 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    ModPlug_Settings settings;
    /* We accept file based on extension match */
    if( !p_demux->b_force )
    {
        const char *psz_ext = p_demux->psz_file ? strrchr( p_demux->psz_file, '.' )
                                                : NULL;
        if( psz_ext )
            psz_ext++;
        if( Validate( p_demux, psz_ext ) )
        {
            msg_Dbg( p_demux, "MOD validation failed (ext=%s)", psz_ext ? psz_ext : "");
            return VLC_EGENERIC;
        }
    }
    const int64_t i_size = stream_Size( p_demux->s );
    if( i_size <= 0 || i_size >= MOD_MAX_FILE_SIZE )
        return VLC_EGENERIC;
    /* Fill p_demux field */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    msg_Dbg( p_demux, "loading complete file (could be long)" );
    p_sys->i_data = i_size;
    p_sys->p_data = malloc( p_sys->i_data );
    if( p_sys->p_data )
        p_sys->i_data = stream_Read( p_demux->s, p_sys->p_data, p_sys->i_data );
    if( p_sys->i_data <= 0 || !p_sys->p_data )
    {
        msg_Err( p_demux, "failed to read the complete file" );
        free( p_sys->p_data );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* Configure modplug before loading the file */
    vlc_mutex_lock( &libmodplug_lock );
    ModPlug_GetSettings( &settings );
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
    settings.mChannels = 2;
    settings.mBits = 16;
    settings.mFrequency = 44100;
    settings.mResamplingMode = MODPLUG_RESAMPLE_FIR;
    if( var_InheritBool( p_demux, "mod-noisereduction" ) )
        settings.mFlags |= MODPLUG_ENABLE_NOISE_REDUCTION;
    if( var_InheritBool( p_demux, "mod-reverb" ) )
        settings.mFlags |= MODPLUG_ENABLE_REVERB;
    settings.mReverbDepth = var_InheritInteger( p_demux, "mod-reverb-level" );
    settings.mReverbDelay = var_InheritInteger( p_demux, "mod-reverb-delay" );
    if( var_InheritBool( p_demux, "mod-megabass" ) )
        settings.mFlags |= MODPLUG_ENABLE_MEGABASS;
    settings.mBassAmount = var_InheritInteger( p_demux, "mod-megabass-level" );
    settings.mBassRange = var_InheritInteger( p_demux, "mod-megabass-range" );
    if( var_InheritBool( p_demux, "mod-surround" ) )
        settings.mFlags |= MODPLUG_ENABLE_SURROUND;
    settings.mSurroundDepth = var_InheritInteger( p_demux, "mod-surround-level" );
    settings.mSurroundDelay = var_InheritInteger( p_demux, "mod-surround-delay" );
    ModPlug_SetSettings( &settings );
    p_sys->f = ModPlug_Load( p_sys->p_data, p_sys->i_data );
    vlc_mutex_unlock( &libmodplug_lock );
    if( !p_sys->f )
    {
        msg_Err( p_demux, "failed to understand the file" );
        /* we try to seek to recover for other plugin */
        stream_Seek( p_demux->s, 0 );
        free( p_sys->p_data );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* init time */
    date_Init( &p_sys->pts, settings.mFrequency, 1 );
    date_Set( &p_sys->pts, 0 );
    p_sys->i_length = ModPlug_GetLength( p_sys->f ) * INT64_C(1000);
    msg_Dbg( p_demux, "MOD loaded name=%s lenght=%"PRId64"ms",
             ModPlug_GetName( p_sys->f ),
             p_sys->i_length );
#ifdef WORDS_BIGENDIAN
    es_format_Init( &p_sys->fmt, AUDIO_ES, VLC_FOURCC( 't', 'w', 'o', 's' ) );
#else
    es_format_Init( &p_sys->fmt, AUDIO_ES, VLC_FOURCC( 'a', 'r', 'a', 'w' ) );
#endif
    p_sys->fmt.audio.i_rate = settings.mFrequency;
    p_sys->fmt.audio.i_channels = settings.mChannels;
    p_sys->fmt.audio.i_bitspersample = settings.mBits;
    p_sys->es = es_out_Add( p_demux->out, &p_sys->fmt );
    return VLC_SUCCESS;
}
