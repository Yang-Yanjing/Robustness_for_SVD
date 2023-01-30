#endif
static int parse_Manifest( stream_t *s )
{
    stream_sys_t *p_sys = s->p_sys;
    xml_t *vlc_xml = NULL;
    xml_reader_t *vlc_reader = NULL;
    int type = UNKNOWN_ES;
    const char *name, *value;
    stream_t *st = s->p_source;
    msg_Dbg( s, "Manifest parsing\n" );
    vlc_xml = xml_Create( st );
    if( !vlc_xml )
    {
        msg_Err( s, "Failed to open XML parser" );
        return VLC_EGENERIC;
    }
    vlc_reader = xml_ReaderCreate( vlc_xml, st );
    if( !vlc_reader )
    {
        msg_Err( s, "Failed to open source for parsing" );
        xml_Delete( vlc_xml );
        return VLC_EGENERIC;
    }
    const char *node;
    uint8_t *WaveFormatEx;
    sms_stream_t *sms = NULL;
    quality_level_t *ql = NULL;
    int64_t start_time = 0, duration = 0;
    int64_t computed_start_time = 0, computed_duration = 0;
    unsigned next_track_id = 1;
    unsigned next_qid = 1;
    int loop_count = 0;
    bool b_weird = false;
#define TIMESCALE 10000000
    while( (type = xml_ReaderNextNode( vlc_reader, &node )) > 0 )
    {
        switch( type )
        {
            case XML_READER_STARTELEM:
                if( !strcmp( node, "SmoothStreamingMedia" ) )
                {
                    while( (name = xml_ReaderNextAttr( vlc_reader, &value )) )
                    {
                        if( !strcmp( name, "Duration" ) )
                            p_sys->vod_duration = strtoull( value, NULL, 10 );
                        else if( !strcmp( name, "TimeScale" ) )
                            p_sys->timescale = strtoull( value, NULL, 10 );
                    }
                    if( !p_sys->timescale )
                        p_sys->timescale = TIMESCALE;
                }
                else if( !strcmp( node, "StreamIndex" ) )
                {
                    sms_Free( sms );
                    sms = sms_New();
                    if( unlikely( !sms ) )
                    {
                        xml_ReaderDelete( vlc_reader );
                        xml_Delete( vlc_xml );
                        return VLC_ENOMEM;
                    }
                    sms->id = next_track_id;
                    next_track_id++;
                    while( (name = xml_ReaderNextAttr( vlc_reader, &value )) )
                    {
                        if( !strcmp( name, "Type" ) )
                        {
                            if( !strcmp( value, "video" ) )
                                sms->type = VIDEO_ES;
                            else if( !strcmp( value, "audio" ) )
                                sms->type = AUDIO_ES;
                            else if( !strcmp( value, "text" ) )
                                sms->type = SPU_ES;
                        }
                        else if( !strcmp( name, "Name" ) )
                            sms->name = strdup( value );
                        else if( !strcmp( name, "TimeScale" ) )
                            sms->timescale = strtoull( value, NULL, 10 );
                        else if( !strcmp( name, "FourCC" ) )
                            sms->default_FourCC =
                                VLC_FOURCC( value[0], value[1], value[2], value[3] );
                        else if( !strcmp( name, "Chunks" ) )
                        {
                            sms->vod_chunks_nb = strtol( value, NULL, 10 );
                            if( sms->vod_chunks_nb == 0 ) /* live */
                                sms->vod_chunks_nb = UINT32_MAX;
                        }
                        else if( !strcmp( name, "QualityLevels" ) )
                            sms->qlevel_nb = strtoul( value, NULL, 10 );
                        else if( !strcmp( name, "Url" ) )
                            sms->url_template = strdup(value);
                    }
                    if( !sms->timescale )
                        sms->timescale = TIMESCALE;
                    if( !sms->name )
                    {
                        if( sms->type == VIDEO_ES )
                            sms->name = strdup( "video" );
                        else if( sms->type == AUDIO_ES )
                            sms->name = strdup( "audio" );
                        else if( sms->type == SPU_ES )
                            sms->name = strdup( "text" );
                    }
                }
                else if( !strcmp( node, "QualityLevel" ) )
                {
                    if ( !sms )
                        break;
                    ql = ql_New();
                    if( !ql )
                    {
                        sms_Free( sms );
                        xml_ReaderDelete( vlc_reader );
                        xml_Delete( vlc_xml );
                        return VLC_ENOMEM;
                    }
                    ql->id = next_qid;
                    next_qid++;
                    while( (name = xml_ReaderNextAttr( vlc_reader, &value )) )
                    {
                        if( !strcmp( name, "Index" ) )
                            ql->Index = strtol( value, NULL, 10 );
                        else if( !strcmp( name, "Bitrate" ) )
                            ql->Bitrate = strtoull( value, NULL, 10 );
                        else if( !strcmp( name, "PacketSize" ) )
                            ql->nBlockAlign = strtoull( value, NULL, 10 );
                        else if( !strcmp( name, "FourCC" ) )
                            ql->FourCC = VLC_FOURCC( value[0], value[1],
                                                     value[2], value[3] );
                        else if( !strcmp( name, "CodecPrivateData" ) )
                            ql->CodecPrivateData = strdup( value );
                        else if( !strcmp( name, "WaveFormatEx" ) )
                        {
                            WaveFormatEx = decode_string_hex_to_binary( value );
                            uint16_t data_len = ((uint16_t *)WaveFormatEx)[8];
                            ql->CodecPrivateData = strndup( value + 36, data_len * 2 );
                            uint16_t wf_tag = ((uint16_t *)WaveFormatEx)[0];
                            wf_tag_to_fourcc( wf_tag, &ql->FourCC, NULL );
                            ql->Channels = ((uint16_t *)WaveFormatEx)[1];
                            ql->SamplingRate = ((uint32_t *)WaveFormatEx)[1];
                            ql->nBlockAlign = ((uint16_t *)WaveFormatEx)[6];
                            ql->BitsPerSample = ((uint16_t *)WaveFormatEx)[7];
                            free( WaveFormatEx );
                        }
                        else if( !strcmp( name, "MaxWidth" ) || !strcmp( name, "Width" ) )
                            ql->MaxWidth = strtoul( value, NULL, 10 );
                        else if( !strcmp( name, "MaxHeight" ) || !strcmp( name, "Height" ) )
                            ql->MaxHeight = strtoul( value, NULL, 10 );
                        else if( !strcmp( name, "Channels" ) )
                            ql->Channels = strtoul( value, NULL, 10 );
                        else if( !strcmp( name, "SamplingRate" ) )
                            ql->SamplingRate = strtoul( value, NULL, 10 );
                        else if( !strcmp( name, "BitsPerSample" ) )
                            ql->BitsPerSample = strtoul( value, NULL, 10 );
                    }
                    vlc_array_append( sms->qlevels, ql );
                }
                else if ( !strcmp( node, "Content" ) && sms && !sms->url_template )
                {
                    /* empty(@Url) && ./Content == manifest embedded content */
                    sms_Free( sms );
                    sms = NULL;
                }
                else if( !strcmp( node, "c" ) )
                {
                    if ( !sms )
                        break;
                    loop_count++;
                    start_time = duration = -1;
                    while( (name = xml_ReaderNextAttr( vlc_reader, &value )) )
                    {
                        if( !strcmp( name, "t" ) )
                            start_time = strtoull( value, NULL, 10 );
                        if( !strcmp( name, "d" ) )
                            duration = strtoull( value, NULL, 10 );
                    }
                    if( start_time == -1 )
                    {
                        assert( duration != -1 );
                        computed_start_time += computed_duration;
                        computed_duration = duration;
                    }
                    else if( duration == -1 )
                    {
                        assert( start_time != -1 );
                        /* Handle weird Manifests which give only the start time
                         * of the first segment. In those cases, we have to look
                         * at the start time of the second segment to compute
                         * the duration of the first one. */
                        if( loop_count == 1 )
                        {
                            b_weird = true;
                            computed_start_time = start_time;
                            continue;
                        }
                        computed_duration = start_time - computed_start_time;
                        if( !b_weird )
                            computed_start_time = start_time;
                    }
                    else
                    {
                        if( b_weird )
                            computed_duration = start_time - computed_start_time;
                        else
                        {
                            computed_start_time = start_time;
                            computed_duration = duration;
                        }
                    }
                    if( unlikely( chunk_New( sms, computed_duration,
                                        computed_start_time ) == NULL ) )
                    {
                        sms_Free( sms );
                        xml_ReaderDelete( vlc_reader );
                        xml_Delete( vlc_xml );
                        return VLC_ENOMEM;
                    }
                    if( b_weird && start_time != -1 )
                        computed_start_time = start_time;
                }
                break;
            case XML_READER_ENDELEM:
                if( strcmp( node, "StreamIndex" ) )
                    break;
                if ( sms )
                {
                    vlc_array_append( p_sys->sms_streams, sms );
                    computed_start_time = 0;
                    computed_duration = 0;
                    loop_count = 0;
                    if( b_weird && !chunk_New( sms, computed_duration, computed_start_time ) )
                    {
                        sms_Free( sms );
                        xml_ReaderDelete( vlc_reader );
                        xml_Delete( vlc_xml );
                        return VLC_ENOMEM;
                    }
                    b_weird = false;
                    next_qid = 1;
                    if( sms->qlevel_nb == 0 )
                        sms->qlevel_nb = vlc_array_count( sms->qlevels );
                    sms = NULL;
                }
                break;
            case XML_READER_TEXT:
                break;
            default:
                sms_Free( sms );
                xml_ReaderDelete( vlc_reader );
                xml_Delete( vlc_xml );
                return VLC_EGENERIC;
        }
    }
#undef TIMESCALE
    sms_Free( sms );
    xml_ReaderDelete( vlc_reader );
    xml_Delete( vlc_xml );
    return VLC_SUCCESS;
}
