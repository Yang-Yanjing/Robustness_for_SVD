}
static inline int64_t get_lead( stream_t *s )
{
    stream_sys_t *p_sys = s->p_sys;
    int64_t lead = 0;
    int64_t alead = p_sys->download.lead[es_cat_to_index( AUDIO_ES )];
    int64_t vlead = p_sys->download.lead[es_cat_to_index( VIDEO_ES )];
    bool video = SMS_GET_SELECTED_ST( VIDEO_ES ) ? true : false;
    bool audio = SMS_GET_SELECTED_ST( AUDIO_ES ) ? true : false;
    if( video && audio )
        lead = __MIN( vlead, alead );
    else if( video )
        lead = vlead;
    else
        lead = alead;
    lead -= p_sys->playback.toffset;
    return lead;
}
