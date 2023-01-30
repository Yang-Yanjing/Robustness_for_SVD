 */
static es_out_id_t *EsOutAdd( es_out_t *out, const es_format_t *fmt )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    if( fmt->i_group < 0 )
    {
        msg_Err( p_input, "invalid group number" );
        return NULL;
    }
    es_out_id_t   *es = malloc( sizeof( *es ) );
    es_out_pgrm_t *p_pgrm;
    int i;
    if( !es )
        return NULL;
    vlc_mutex_lock( &p_sys->lock );
    /* Search the program */
    p_pgrm = EsOutProgramFind( out, fmt->i_group );
    if( !p_pgrm )
    {
        vlc_mutex_unlock( &p_sys->lock );
        free( es );
        return NULL;
    }
    /* Increase ref count for program */
    p_pgrm->i_es++;
    /* Set up ES */
    es->p_pgrm = p_pgrm;
    es_format_Copy( &es->fmt, fmt );
    if( es->fmt.i_id < 0 )
        es->fmt.i_id = out->p_sys->i_id;
    if( !es->fmt.i_original_fourcc )
        es->fmt.i_original_fourcc = es->fmt.i_codec;
    if( es->fmt.i_cat == AUDIO_ES )
        es->fmt.i_codec = vlc_fourcc_GetCodecAudio( es->fmt.i_codec,
                                                    es->fmt.audio.i_bitspersample );
    else
        es->fmt.i_codec = vlc_fourcc_GetCodec( es->fmt.i_cat,
                                               es->fmt.i_codec );
    if( es->fmt.i_cat == VIDEO_ES
     && (!es->fmt.video.i_visible_width || !es->fmt.video.i_visible_height))
    {
        es->fmt.video.i_visible_width = es->fmt.video.i_width;
        es->fmt.video.i_visible_height = es->fmt.video.i_height;
    }
    es->i_id = es->fmt.i_id;
    es->i_meta_id = out->p_sys->i_id;
    es->b_scrambled = false;
    switch( es->fmt.i_cat )
    {
    case AUDIO_ES:
    {
        audio_replay_gain_t rg;
        es->i_channel = p_sys->i_audio;
        memset( &rg, 0, sizeof(rg) );
        vlc_mutex_lock( &p_input->p->p_item->lock );
        vlc_audio_replay_gain_MergeFromMeta( &rg, p_input->p->p_item->p_meta );
        vlc_mutex_unlock( &p_input->p->p_item->lock );
        for( i = 0; i < AUDIO_REPLAY_GAIN_MAX; i++ )
        {
            if( !es->fmt.audio_replay_gain.pb_peak[i] )
            {
                es->fmt.audio_replay_gain.pb_peak[i] = rg.pb_peak[i];
                es->fmt.audio_replay_gain.pf_peak[i] = rg.pf_peak[i];
            }
            if( !es->fmt.audio_replay_gain.pb_gain[i] )
            {
                es->fmt.audio_replay_gain.pb_gain[i] = rg.pb_gain[i];
                es->fmt.audio_replay_gain.pf_gain[i] = rg.pf_gain[i];
            }
        }
        break;
    }
    case VIDEO_ES:
        es->i_channel = p_sys->i_video;
        if( es->fmt.video.i_frame_rate && es->fmt.video.i_frame_rate_base )
            vlc_ureduce( &es->fmt.video.i_frame_rate,
                         &es->fmt.video.i_frame_rate_base,
                         es->fmt.video.i_frame_rate,
                         es->fmt.video.i_frame_rate_base, 0 );
        break;
    case SPU_ES:
        es->i_channel = p_sys->i_sub;
        break;
    default:
        es->i_channel = 0;
        break;
    }
    es->psz_language = LanguageGetName( es->fmt.psz_language ); /* remember so we only need to do it once */
    es->psz_language_code = LanguageGetCode( es->fmt.psz_language );
    es->p_dec = NULL;
    es->p_dec_record = NULL;
    for( i = 0; i < 4; i++ )
        es->pb_cc_present[i] = false;
    es->p_master = NULL;
    if( es->p_pgrm == p_sys->p_pgrm )
        EsOutESVarUpdate( out, es, false );
    /* Select it if needed */
    EsOutSelect( out, es, false );
    TAB_APPEND( out->p_sys->i_es, out->p_sys->es, es );
    p_sys->i_id++;  /* always incremented */
    switch( es->fmt.i_cat )
    {
        case AUDIO_ES:
            p_sys->i_audio++;
            break;
        case SPU_ES:
            p_sys->i_sub++;
            break;
        case VIDEO_ES:
            p_sys->i_video++;
            break;
    }
    EsOutUpdateInfo( out, es, &es->fmt, NULL );
    if( es->b_scrambled )
        EsOutProgramUpdateScrambled( out, es->p_pgrm );
    vlc_mutex_unlock( &p_sys->lock );
    return es;
}
