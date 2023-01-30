}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t  *id;
    const char        *psz_access;
    const char        *psz_mux;
    char              *psz_dst;
    sout_access_out_t *p_access;
    sout_mux_t        *p_mux;
    /* *** get access name *** */
    if( p_fmt->i_cat == AUDIO_ES && p_sys->psz_access_audio && *p_sys->psz_access_audio )
    {
        psz_access = p_sys->psz_access_audio;
    }
    else if( p_fmt->i_cat == VIDEO_ES && p_sys->psz_access_video && *p_sys->psz_access_video )
    {
        psz_access = p_sys->psz_access_video;
    }
    else
    {
        psz_access = p_sys->psz_access;
    }
    /* *** get mux name *** */
    if( p_fmt->i_cat == AUDIO_ES && p_sys->psz_mux_audio && *p_sys->psz_mux_audio )
    {
        psz_mux = p_sys->psz_mux_audio;
    }
    else if( p_fmt->i_cat == VIDEO_ES && p_sys->psz_mux_video && *p_sys->psz_mux_video )
    {
        psz_mux = p_sys->psz_mux_video;
    }
    else
    {
        psz_mux = p_sys->psz_mux;
    }
    /* Get url (%d expanded as a codec count, %c expanded as codec fcc ) */
    if( p_fmt->i_cat == AUDIO_ES && p_sys->psz_dst_audio && *p_sys->psz_dst_audio )
    {
        psz_dst = es_print_url( p_sys->psz_dst_audio, p_fmt->i_codec,
                                p_sys->i_count_audio, psz_access, psz_mux );
    }
    else if( p_fmt->i_cat == VIDEO_ES && p_sys->psz_dst_video && *p_sys->psz_dst_video )
    {
        psz_dst = es_print_url( p_sys->psz_dst_video, p_fmt->i_codec,
                                p_sys->i_count_video, psz_access, psz_mux );
    }
    else
    {
        int i_count;
        if( p_fmt->i_cat == VIDEO_ES )
        {
            i_count = p_sys->i_count_video;
        }
        else if( p_fmt->i_cat == AUDIO_ES )
        {
            i_count = p_sys->i_count_audio;
        }
        else
        {
            i_count = p_sys->i_count;
        }
        psz_dst = es_print_url( p_sys->psz_dst, p_fmt->i_codec,
                                i_count, psz_access, psz_mux );
    }
    p_sys->i_count++;
    if( p_fmt->i_cat == VIDEO_ES )
    {
        p_sys->i_count_video++;
    }
    else if( p_fmt->i_cat == AUDIO_ES )
    {
        p_sys->i_count_audio++;
    }
    msg_Dbg( p_stream, "creating `%s/%s://%s'",
             psz_access, psz_mux, psz_dst );
    /* *** find and open appropriate access module *** */
    p_access = sout_AccessOutNew( p_stream, psz_access, psz_dst );
    if( p_access == NULL )
    {
        msg_Err( p_stream, "no suitable sout access module for `%s/%s://%s'",
                 psz_access, psz_mux, psz_dst );
        dialog_Fatal( p_stream,
                    _("Streaming / Transcoding failed"),
                    _("There is no suitable stream-output access module for \"%s/%s://%s\"."),
                          psz_access,
                          psz_mux, psz_dst );
        free( psz_dst );
        return( NULL );
    }
    /* *** find and open appropriate mux module *** */
    p_mux = sout_MuxNew( p_stream->p_sout, psz_mux, p_access );
    if( p_mux == NULL )
    {
        msg_Err( p_stream, "no suitable sout mux module for `%s/%s://%s'",
                 psz_access, psz_mux, psz_dst );
        dialog_Fatal( p_stream,
                        _("Streaming / Transcoding failed"),
                        _("There is no suitable stream-output access module "\
                          "for \"%s/%s://%s\"."),
                          psz_access, psz_mux, psz_dst );
        sout_AccessOutDelete( p_access );
        free( psz_dst );
        return( NULL );
    }
    free( psz_dst );
    id = malloc( sizeof( sout_stream_id_sys_t ) );
    if( !id )
    {
        sout_MuxDelete( p_mux );
        sout_AccessOutDelete( p_access );
        return NULL;
    }
    id->p_mux = p_mux;
    id->p_input = sout_MuxAddStream( p_mux, p_fmt );
    if( id->p_input == NULL )
    {
        sout_MuxDelete( p_mux );
        sout_AccessOutDelete( p_access );
        free( id );
        return NULL;
    }
    if( !sout_AccessOutCanControlPace( p_access ) )
        p_stream->p_sout->i_out_pace_nocontrol++;
    return id;
}
