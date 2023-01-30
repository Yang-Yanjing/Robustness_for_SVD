 */
static void IVTCLowLevelDetect( filter_t *p_filter )
{
    assert( p_filter != NULL );
    filter_sys_t *p_sys = p_filter->p_sys;
    ivtc_sys_t *p_ivtc  = &p_sys->ivtc;
    picture_t *p_curr = p_sys->pp_history[1];
    picture_t *p_next = p_sys->pp_history[2];
    assert( p_next != NULL );
    assert( p_curr != NULL );
    /* Compute interlace scores for TNBN, TNBC and TCBN.
        Note that p_next contains TNBN. */
    p_ivtc->pi_scores[FIELD_PAIR_TNBN] = CalculateInterlaceScore( p_next,
                                                                  p_next );
    p_ivtc->pi_scores[FIELD_PAIR_TNBC] = CalculateInterlaceScore( p_next,
                                                                  p_curr );
    p_ivtc->pi_scores[FIELD_PAIR_TCBN] = CalculateInterlaceScore( p_curr,
                                                                  p_next );
    int i_top = 0, i_bot = 0;
    int i_motion = EstimateNumBlocksWithMotion(p_curr, p_next, &i_top, &i_bot);
    p_ivtc->pi_motion[IVTC_LATEST] = i_motion;
    /* If one field changes "clearly more" than the other, we know the
       less changed one is a likely duplicate.
       Threshold 1/2 is too low for some scenes (e.g. pan of the space junk
       at beginning of The Third ep. 1, right after the OP). Thus, we use 2/3,
       which seems to work.
    */
    p_ivtc->pi_top_rep[IVTC_LATEST] = (i_top <= 2*i_bot/3);
    p_ivtc->pi_bot_rep[IVTC_LATEST] = (i_bot <= 2*i_top/3);
}
