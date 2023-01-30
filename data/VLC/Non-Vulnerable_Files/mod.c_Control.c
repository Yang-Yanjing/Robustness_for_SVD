 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    double f, *pf;
    int64_t i64, *pi64;
    switch( i_query )
    {
    case DEMUX_GET_POSITION:
        pf = (double*) va_arg( args, double* );
        if( p_sys->i_length > 0 )
        {
            double current = date_Get( &p_sys->pts );
            double length = p_sys->i_length;
            *pf = current / length;
            return VLC_SUCCESS;
        }
        return VLC_EGENERIC;
    case DEMUX_SET_POSITION:
        f = (double) va_arg( args, double );
        i64 = f * p_sys->i_length;
        if( i64 >= 0 && i64 <= p_sys->i_length )
        {
            ModPlug_Seek( p_sys->f, i64 / 1000 );
            date_Set( &p_sys->pts, i64 );
            return VLC_SUCCESS;
        }
        return VLC_EGENERIC;
    case DEMUX_GET_TIME:
        pi64 = (int64_t*)va_arg( args, int64_t * );
        *pi64 = date_Get( &p_sys->pts );
        return VLC_SUCCESS;
    case DEMUX_GET_LENGTH:
        pi64 = (int64_t*)va_arg( args, int64_t * );
        *pi64 = p_sys->i_length;
        return VLC_SUCCESS;
    case DEMUX_SET_TIME:
        i64 = (int64_t)va_arg( args, int64_t );
        if( i64 >= 0 && i64 <= p_sys->i_length )
        {
            ModPlug_Seek( p_sys->f, i64 / 1000 );
            date_Set( &p_sys->pts, i64 );
            return VLC_SUCCESS;
        }
        return VLC_EGENERIC;
    case DEMUX_HAS_UNSUPPORTED_META:
    {
        bool *pb_bool = (bool*)va_arg( args, bool* );
        *pb_bool = false; /* FIXME I am not sure of this one */
        return VLC_SUCCESS;
    }
    case DEMUX_GET_META:
    {
        vlc_meta_t *p_meta = (vlc_meta_t *)va_arg( args, vlc_meta_t* );
        unsigned i_num_samples = ModPlug_NumSamples( p_sys->f ),
                 i_num_instruments = ModPlug_NumInstruments( p_sys->f );
        unsigned i_num_patterns = ModPlug_NumPatterns( p_sys->f ),
                 i_num_channels = ModPlug_NumChannels( p_sys->f );
        //      unsigned modType = ModPlug_GetModuleType( p_sys->f );
        char psz_temp[2048]; /* 32 * 240 max, but only need start  */
        char *psz_module_info, *psz_instrument_info;
        unsigned i_temp_index = 0;
        const char *psz_name = ModPlug_GetName( p_sys->f );
        if( psz_name && *psz_name && IsUTF8( psz_name ) )
            vlc_meta_SetTitle( p_meta, psz_name );
        /* Comment field from artist - not in every type of MOD */
        psz_name = ModPlug_GetMessage( p_sys->f );
        if( psz_name && *psz_name && IsUTF8( psz_name ) )
            vlc_meta_SetDescription( p_meta, psz_name );
        /* Instruments only in newer MODs - so don't show if 0 */
        if( asprintf( &psz_instrument_info, ", %i Instruments",
                      i_num_instruments ) >= 0 )
        {
            if( asprintf( &psz_module_info,
                          "%i Channels, %i Patterns\n"
                          "%i Samples%s\n",
                          i_num_channels, i_num_patterns, i_num_samples,
                          ( i_num_instruments ? psz_instrument_info : "" ) ) >= 0 )
            {
                vlc_meta_AddExtra( p_meta, "Module Information",
                                   psz_module_info );
                free( psz_module_info );
            }
            free( psz_instrument_info );
        }
        /* Make list of instruments (XM, IT, etc) */
        if( i_num_instruments )
        {
            i_temp_index = 0;
            for( unsigned i = 0; i < i_num_instruments && i_temp_index < sizeof(psz_temp); i++ )
            {
                char lBuffer[33];
                ModPlug_InstrumentName( p_sys->f, i, lBuffer );
                if ( !lBuffer[0] || !IsUTF8( lBuffer ) ) continue;
                i_temp_index += snprintf( &psz_temp[i_temp_index], sizeof(psz_temp) - i_temp_index, "%s\n", lBuffer );
            }
            vlc_meta_AddExtra( p_meta, "Instruments", psz_temp );
        }
        /* Make list of samples */
        for( unsigned int i = 0; i < i_num_samples && i_temp_index < sizeof(psz_temp); i++ )
        {
            char psz_buffer[33];
            ModPlug_SampleName( p_sys->f, i, psz_buffer );
            if ( !psz_buffer[0] || !IsUTF8( psz_buffer ) ) continue;
            i_temp_index += snprintf( &psz_temp[i_temp_index], sizeof(psz_temp) - i_temp_index, "%s\n", psz_buffer );
        }
        vlc_meta_AddExtra( p_meta, "Samples", psz_temp );
        return VLC_SUCCESS;
    }
    case DEMUX_GET_FPS: /* meaningless */
    default:
        return VLC_EGENERIC;
    }
}
