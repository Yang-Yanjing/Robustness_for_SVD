 */
static void IVTCCadenceDetectFinalize( filter_t *p_filter )
{
    assert( p_filter != NULL );
    filter_sys_t *p_sys = p_filter->p_sys;
    ivtc_sys_t *p_ivtc  = &p_sys->ivtc;
    /* In practice "vektor" is more reliable than "scores", but it may
       take longer to lock on. Thus, we prefer "vektor" if its reliable bit
       is set, then "scores", and finally just give up.
       For progressive sequences, "vektor" outputs "3, -, 3, -, ...",
       because the repeated progressive position is an inconsistent prediction.
       In this case, "scores" fills in the blanks. (This particular task
       could also be done without another cadence detector, by just
       detecting the alternating pattern of "3" and no result.)
    */
    int pos = CADENCE_POS_INVALID;
    if( p_ivtc->pb_v_reliable[IVTC_LATEST] )
        pos = p_ivtc->pi_v_cadence_pos[IVTC_LATEST];
    else if( p_ivtc->pb_s_reliable[IVTC_LATEST] )
        pos = p_ivtc->pi_s_cadence_pos[IVTC_LATEST];
    p_ivtc->pi_cadence_pos_history[IVTC_LATEST] = pos;
}
