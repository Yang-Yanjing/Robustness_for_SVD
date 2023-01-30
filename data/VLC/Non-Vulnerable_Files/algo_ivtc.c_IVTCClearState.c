/* See function doc in header. */
void IVTCClearState( filter_t *p_filter )
{
    assert( p_filter != NULL );
    filter_sys_t *p_sys = p_filter->p_sys;
    ivtc_sys_t *p_ivtc = &p_sys->ivtc;
    p_ivtc->i_cadence_pos = CADENCE_POS_INVALID;
    p_ivtc->i_tfd         = TFD_INVALID;
    p_ivtc->b_sequence_valid = false;
    p_ivtc->i_mode     = IVTC_MODE_DETECTING;
    p_ivtc->i_old_mode = IVTC_MODE_DETECTING;
    for( int i = 0; i < IVTC_NUM_FIELD_PAIRS; i++ )
        p_ivtc->pi_scores[i] = 0;
    for( int i = 0; i < IVTC_DETECTION_HISTORY_SIZE; i++ )
    {
        p_ivtc->pi_cadence_pos_history[i] = CADENCE_POS_INVALID;
        p_ivtc->pi_s_cadence_pos[i] = CADENCE_POS_INVALID;
        p_ivtc->pb_s_reliable[i]    = false;
        p_ivtc->pi_v_cadence_pos[i] = CADENCE_POS_INVALID;
        p_ivtc->pb_v_reliable[i]    = false;
        p_ivtc->pi_v_raw[i]         = VEKTOR_CADENCE_POS_ALL;
        p_ivtc->pi_top_rep[i] =  0;
        p_ivtc->pi_bot_rep[i] =  0;
        p_ivtc->pi_motion[i]  = -1;
        p_ivtc->pb_all_progressives[i] = false;
        p_ivtc->pi_final_scores[i] = 0;
    }
}
