 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    audio_format_t *audio_in  = &p_filter->fmt_in.audio;
    audio_format_t *audio_out = &p_filter->fmt_out.audio;
    if( ( audio_in->i_format != audio_out->i_format ) ||
        ( audio_in->i_rate != audio_out->i_rate ) )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(filter_sys_t) );
    if( unlikely( p_sys == NULL ) )
        return VLC_ENOMEM;
    /* get number of and layout of input channels */
    uint32_t i_output_physical = 0;
    uint8_t pi_map_ch[ AOUT_CHAN_MAX ] = { 0 }; /* which out channel each in channel is mapped to */
    p_sys->b_normalize = var_InheritBool( p_this, REMAP_CFG "normalize" );
    for( uint8_t in_ch = 0, wg4_i = 0; in_ch < audio_in->i_channels; in_ch++, wg4_i++ )
    {
        /* explode in_channels in the right order */
        while( ( audio_in->i_physical_channels & pi_vlc_chan_order_wg4[ wg4_i ] ) == 0 )
        {
            wg4_i++;
            assert( wg4_i < sizeof( pi_vlc_chan_order_wg4 )/sizeof( pi_vlc_chan_order_wg4[0] ) );
        }
        unsigned channel_wg4idx_len = sizeof( channel_wg4idx )/sizeof( channel_wg4idx[0] );
        uint8_t *pi_chnidx = memchr( channel_wg4idx, wg4_i, channel_wg4idx_len );
        assert( pi_chnidx != NULL );
        uint8_t chnidx = pi_chnidx - channel_wg4idx;
        uint8_t out_idx = var_InheritInteger( p_this, channel_name[chnidx] );
        pi_map_ch[in_ch] = channel_wg4idx[ out_idx ];
        i_output_physical |= channel_flag[ out_idx ];
    }
    i_output_physical = CanonicaliseChannels( i_output_physical );
    audio_out->i_physical_channels = i_output_physical;
    aout_FormatPrepare( audio_out );
    /* condense out_channels */
    uint8_t out_ch_sorted[ AOUT_CHAN_MAX ];
    for( uint8_t i = 0, wg4_i = 0; i < audio_out->i_channels; i++, wg4_i++ )
    {
        while( ( audio_out->i_physical_channels & pi_vlc_chan_order_wg4[ wg4_i ] ) == 0 )
        {
            wg4_i++;
            assert( wg4_i < sizeof( pi_vlc_chan_order_wg4 )/sizeof( pi_vlc_chan_order_wg4[0] ) );
        }
        out_ch_sorted[ i ] = wg4_i;
    }
    bool b_multiple = false; /* whether we need to add channels (multiple in mapped to an out) */
    memset( p_sys->nb_in_ch, 0, sizeof( p_sys->nb_in_ch ) );
    for( uint8_t i = 0; i < audio_in->i_channels; i++ )
    {
        uint8_t wg4_out_ch = pi_map_ch[i];
        uint8_t *pi_out_ch = memchr( out_ch_sorted, wg4_out_ch, audio_out->i_channels );
        assert( pi_out_ch != NULL );
        p_sys->map_ch[i] = pi_out_ch - out_ch_sorted;
        if( ++p_sys->nb_in_ch[ p_sys->map_ch[i] ] > 1 )
            b_multiple = true;
    }
    msg_Dbg( p_filter, "%s '%4.4s'->'%4.4s' %d Hz->%d Hz %s->%s",
             "Remap filter",
             (char *)&audio_in->i_format, (char *)&audio_out->i_format,
             audio_in->i_rate, audio_out->i_rate,
             aout_FormatPrintChannels( audio_in ),
             aout_FormatPrintChannels( audio_out ) );
    p_sys->pf_remap = GetRemapFun( audio_in, b_multiple );
    if( !p_sys->pf_remap )
    {
        msg_Err( p_filter, "Could not decide on %s remap function", b_multiple ? "an add" : "a copy" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_filter->pf_audio_filter = Remap;
    return VLC_SUCCESS;
}
