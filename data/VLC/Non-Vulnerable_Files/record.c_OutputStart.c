}
static void OutputStart( sout_stream_t *p_stream )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    /* */
    if( p_sys->b_drop )
        return;
    /* From now on drop packet that cannot be handled */
    p_sys->b_drop = true;
    /* Detect streams to smart select muxer */
    const char *psz_muxer = NULL;
    const char *psz_extension = NULL;
    /* Look for preferred muxer
     * TODO we could insert transcode in a few cases like
     * s16l <-> s16b
     */
    for( unsigned i = 0; i < sizeof(p_muxers) / sizeof(*p_muxers); i++ )
    {
        bool b_ok;
        if( p_sys->i_id > p_muxers[i].i_es_max )
            continue;
        b_ok = true;
        for( int j = 0; j < p_sys->i_id; j++ )
        {
            es_format_t *p_fmt = &p_sys->id[j]->fmt;
            b_ok = false;
            for( int k = 0; p_muxers[i].codec[k] != 0; k++ )
            {
                if( p_fmt->i_codec == p_muxers[i].codec[k] )
                {
                    b_ok = true;
                    break;
                }
            }
            if( !b_ok )
                break;
        }
        if( !b_ok )
            continue;
        psz_muxer = p_muxers[i].psz_muxer;
        psz_extension = p_muxers[i].psz_extension;
        break;
    }
    /* If failed, brute force our demuxers and select the one that
     * keeps most of our stream */
    if( !psz_muxer || !psz_extension )
    {
        static const char ppsz_muxers[][2][4] = {
            { "avi", "avi" }, { "mp4", "mp4" }, { "ogg", "ogg" },
            { "asf", "asf" }, {  "ts",  "ts" }, {  "ps", "mpg" },
            { "mkv", "mkv" },
#if 0
            // XXX ffmpeg sefault really easily if you try an unsupported codec
            // mov and avi at least segfault
            { "avformat{mux=avi}", "avi" },
            { "avformat{mux=mov}", "mov" },
            { "avformat{mux=mp4}", "mp4" },
            { "avformat{mux=nsv}", "nsv" },
            { "avformat{mux=flv}", "flv" },
#endif
        };
        int i_best = 0;
        int i_best_es = 0;
        msg_Warn( p_stream, "failed to find an adequate muxer, probing muxers" );
        for( unsigned i = 0; i < sizeof(ppsz_muxers) / sizeof(*ppsz_muxers); i++ )
        {
            char *psz_file;
            int i_es;
            psz_file = tempnam( NULL, "vlc" );
            if( !psz_file )
                continue;
            msg_Dbg( p_stream, "probing muxer %s", ppsz_muxers[i][0] );
            i_es = OutputNew( p_stream, ppsz_muxers[i][0], psz_file, NULL );
            if( i_es < 0 )
            {
                vlc_unlink( psz_file );
                free( psz_file );
                continue;
            }
            /* */
            for( int i = 0; i < p_sys->i_id; i++ )
            {
                sout_stream_id_sys_t *id = p_sys->id[i];
                if( id->id )
                    sout_StreamIdDel( p_sys->p_out, id->id );
                id->id = NULL;
            }
            if( p_sys->p_out )
                sout_StreamChainDelete( p_sys->p_out, p_sys->p_out );
            p_sys->p_out = NULL;
            if( i_es > i_best_es )
            {
                i_best_es = i_es;
                i_best = i;
                if( i_best_es >= p_sys->i_id )
                    break;
            }
            vlc_unlink( psz_file );
            free( psz_file );
        }
        /* */
        psz_muxer = ppsz_muxers[i_best][0];
        psz_extension = ppsz_muxers[i_best][1];
        msg_Dbg( p_stream, "using muxer %s with extension %s (%d/%d streams accepted)",
                 psz_muxer, psz_extension, i_best_es, p_sys->i_id );
    }
    /* Create the output */
    if( OutputNew( p_stream, psz_muxer, p_sys->psz_prefix, psz_extension ) < 0 )
    {
        msg_Err( p_stream, "failed to open output");
        return;
    }
    /* Compute highest timestamp of first I over all streams */
    p_sys->i_dts_start = 0;
    for( int i = 0; i < p_sys->i_id; i++ )
    {
        sout_stream_id_sys_t *id = p_sys->id[i];
        block_t *p_block;
        if( !id->id || !id->p_first )
            continue;
        mtime_t i_dts = id->p_first->i_dts;
        for( p_block = id->p_first; p_block != NULL; p_block = p_block->p_next )
        {
            if( p_block->i_flags & BLOCK_FLAG_TYPE_I )
            {
                i_dts = p_block->i_dts;
                break;
            }
        }
        if( i_dts > p_sys->i_dts_start )
            p_sys->i_dts_start = i_dts;
    }
    /* Send buffered data */
    for( int i = 0; i < p_sys->i_id; i++ )
    {
        sout_stream_id_sys_t *id = p_sys->id[i];
        if( !id->id )
            continue;
        block_t *p_block = id->p_first;
        while( p_block )
        {
            block_t *p_next = p_block->p_next;
            p_block->p_next = NULL;
            OutputSend( p_stream, id, p_block );
            p_block = p_next;
        }
        id->p_first = NULL;
        id->pp_last = &id->p_first;
    }
}
