 *****************************************************************************/
decoder_synchro_t * decoder_SynchroInit( decoder_t *p_dec, int i_frame_rate )
{
    decoder_synchro_t * p_synchro = calloc( 1, sizeof(*p_synchro) );
    if( !p_synchro )
        return NULL;
    p_synchro->p_dec = p_dec;
    p_synchro->b_no_skip = !var_InheritBool( p_dec, "skip-frames" );
    p_synchro->b_quiet = var_InheritBool( p_dec, "quiet-synchro" );
    /* We use a fake stream pattern, which is often right. */
    p_synchro->i_n_p = p_synchro->i_eta_p = DEFAULT_NB_P;
    p_synchro->i_n_b = p_synchro->i_eta_b = DEFAULT_NB_B;
    memset( p_synchro->p_tau, 0, 4 * sizeof(mtime_t) );
    memset( p_synchro->pi_meaningful, 0, 4 * sizeof(unsigned int) );
    p_synchro->i_nb_ref = 0;
    p_synchro->i_trash_nb_ref = p_synchro->i_dec_nb_ref = 0;
    p_synchro->current_pts = 1,
    p_synchro->backward_pts = 0;
    p_synchro->i_current_period = p_synchro->i_backward_period = 0;
    p_synchro->i_trashed_pic = p_synchro->i_not_chosen_pic =
        p_synchro->i_pic = 0;
    p_synchro->i_frame_rate = i_frame_rate;
    return p_synchro;
}
