 *****************************************************************************/
bool decoder_SynchroChoose( decoder_synchro_t * p_synchro, int i_coding_type,
                               int i_render_time, bool b_low_delay )
{
#define TAU_PRIME( coding_type )    (p_synchro->p_tau[(coding_type)] \
                                    + (p_synchro->p_tau[(coding_type)] >> 1) \
                                    + p_synchro->i_render_time)
#define S (*p_synchro)
    mtime_t         now, period;
    mtime_t         pts;
    bool      b_decode = 0;
    int       i_current_rate;
    if ( p_synchro->b_no_skip )
        return 1;
    i_current_rate = decoder_GetDisplayRate( p_synchro->p_dec );
    now = mdate();
    period = CLOCK_FREQ * 1001 / p_synchro->i_frame_rate
                     * i_current_rate / INPUT_RATE_DEFAULT;
    p_synchro->i_render_time = i_render_time;
    switch( i_coding_type )
    {
    case I_CODING_TYPE:
        if( b_low_delay )
        {
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.current_pts );
        }
        else if( S.backward_pts )
        {
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.backward_pts );
        }
        else
        {
            /* displaying order : B B P B B I
             *                      ^       ^
             *                      |       +- current picture
             *                      +- current PTS
             */
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.current_pts ) + period * (S.i_n_b + 2);
        }
        if( (1 + S.i_n_p * (S.i_n_b + 1)) * period > S.p_tau[I_CODING_TYPE] )
        {
            b_decode = 1;
        }
        else
        {
            b_decode = (pts - now) > (TAU_PRIME(I_CODING_TYPE) + DELTA);
        }
        if( pts <= VLC_TS_INVALID )
            b_decode = 1;
        if( !b_decode && !p_synchro->b_quiet )
        {
            msg_Warn( p_synchro->p_dec,
                      "synchro trashing I (%"PRId64")", pts - now );
        }
        break;
    case P_CODING_TYPE:
        if( b_low_delay )
        {
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.current_pts );
        }
        else if( S.backward_pts )
        {
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.backward_pts );
        }
        else
        {
            pts = decoder_GetDisplayDate( p_synchro->p_dec, S.current_pts + period * (S.i_n_b + 1) );
        }
        if( p_synchro->i_nb_ref < 1 )
        {
            b_decode = 0;
        }
        else if( (1 + S.i_n_p * (S.i_n_b + 1)) * period >
                S.p_tau[I_CODING_TYPE] )
        {
            if( (S.i_n_b + 1) * period > S.p_tau[P_CODING_TYPE] )
            {
                /* Security in case we're _really_ late */
                b_decode = (pts - now > 0);
            }
            else
            {
                b_decode = (pts - now) > (TAU_PRIME(P_CODING_TYPE) + DELTA);
                /* next I */
                b_decode &= (pts - now
                              + period
                          * ( (S.i_n_p - S.i_eta_p) * (1 + S.i_n_b) - 1 ))
                            > (TAU_PRIME(P_CODING_TYPE)
                                + TAU_PRIME(I_CODING_TYPE) + DELTA);
            }
        }
        else
        {
            b_decode = 0;
        }
        if( p_synchro->i_nb_ref >= 1 && pts <= VLC_TS_INVALID )
            b_decode = 1;
        break;
    case B_CODING_TYPE:
        pts = decoder_GetDisplayDate( p_synchro->p_dec, S.current_pts );
        if( p_synchro->i_nb_ref < 2 )
        {
            b_decode = 0;
        }
        else if( (S.i_n_b + 1) * period > S.p_tau[P_CODING_TYPE] )
        {
            b_decode = (pts - now) > (TAU_PRIME(B_CODING_TYPE) + DELTA);
        }
        else
        {
            b_decode = 0;
        }
        if( p_synchro->i_nb_ref >= 2 && pts <= VLC_TS_INVALID )
            b_decode = 1;
        break;
    }
    if( !b_decode )
    {
        S.i_not_chosen_pic++;
    }
    return( b_decode );
#undef S
#undef TAU_PRIME
}
