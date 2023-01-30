 *****************************************************************************/
void decoder_SynchroNewPicture( decoder_synchro_t * p_synchro, int i_coding_type,
                                int i_repeat_field, mtime_t next_pts,
                                mtime_t next_dts, bool b_low_delay )
{
    mtime_t         period = 1000000 * 1001 / p_synchro->i_frame_rate;
#if 0
    mtime_t         now = mdate();
#endif
    switch( i_coding_type )
    {
    case I_CODING_TYPE:
        if( p_synchro->i_eta_p && p_synchro->i_eta_p != p_synchro->i_n_p )
        {
#if 0
            if( !p_synchro->b_quiet )
                msg_Dbg( p_synchro->p_dec,
                         "stream periodicity changed from P[%d] to P[%d]",
                         p_synchro->i_n_p, p_synchro->i_eta_p );
#endif
            p_synchro->i_n_p = p_synchro->i_eta_p;
        }
        p_synchro->i_eta_p = p_synchro->i_eta_b = 0;
        p_synchro->i_trash_nb_ref = 0;
        if( p_synchro->i_nb_ref < 2 )
            p_synchro->i_dec_nb_ref = p_synchro->i_nb_ref + 1;
        else
            p_synchro->i_dec_nb_ref = p_synchro->i_nb_ref;
#if 0
        if( !p_synchro->b_quiet )
            msg_Dbg( p_synchro->p_dec, "I(%"PRId64") P(%"PRId64")[%d] B(%"PRId64")"
                  "[%d] YUV(%"PRId64") : trashed %d:%d/%d",
                  p_synchro->p_tau[I_CODING_TYPE],
                  p_synchro->p_tau[P_CODING_TYPE],
                  p_synchro->i_n_p,
                  p_synchro->p_tau[B_CODING_TYPE],
                  p_synchro->i_n_b,
                  p_synchro->i_render_time,
                  p_synchro->i_not_chosen_pic,
                  p_synchro->i_trashed_pic -
                  p_synchro->i_not_chosen_pic,
                  p_synchro->i_pic );
        p_synchro->i_trashed_pic = p_synchro->i_not_chosen_pic
            = p_synchro->i_pic = 0;
#else
        if( p_synchro->i_pic >= 100 )
        {
            if( !p_synchro->b_quiet && p_synchro->i_trashed_pic != 0 )
                msg_Dbg( p_synchro->p_dec, "decoded %d/%d pictures",
                         p_synchro->i_pic
                           - p_synchro->i_trashed_pic,
                         p_synchro->i_pic );
            p_synchro->i_trashed_pic = p_synchro->i_not_chosen_pic
                = p_synchro->i_pic = 0;
        }
#endif
        break;
    case P_CODING_TYPE:
        p_synchro->i_eta_p++;
        if( p_synchro->i_eta_b
             && p_synchro->i_eta_b != p_synchro->i_n_b )
        {
#if 0
            if( !p_synchro->b_quiet )
                msg_Dbg( p_synchro->p_dec,
                         "stream periodicity changed from B[%d] to B[%d]",
                         p_synchro->i_n_b, p_synchro->i_eta_b );
#endif
            p_synchro->i_n_b = p_synchro->i_eta_b;
        }
        p_synchro->i_eta_b = 0;
        p_synchro->i_dec_nb_ref = 2;
        p_synchro->i_trash_nb_ref = 0;
        break;
    case B_CODING_TYPE:
        p_synchro->i_eta_b++;
        p_synchro->i_dec_nb_ref = p_synchro->i_trash_nb_ref
            = p_synchro->i_nb_ref;
        break;
    }
    p_synchro->current_pts += p_synchro->i_current_period
                                        * (period >> 1);
#define PTS_THRESHOLD   (period >> 2)
    if( i_coding_type == B_CODING_TYPE || b_low_delay )
    {
        /* A video frame can be displayed 1, 2 or 3 times, according to
         * repeat_first_field, top_field_first, progressive_sequence and
         * progressive_frame. */
        p_synchro->i_current_period = i_repeat_field;
        if( next_pts )
        {
            if( (next_pts - p_synchro->current_pts
                    > PTS_THRESHOLD
                  || p_synchro->current_pts - next_pts
                    > PTS_THRESHOLD) && !p_synchro->b_quiet )
            {
                msg_Warn( p_synchro->p_dec, "decoder synchro warning: pts != "
                          "current_date (%"PRId64")",
                          p_synchro->current_pts
                              - next_pts );
            }
            p_synchro->current_pts = next_pts;
        }
    }
    else
    {
        p_synchro->i_current_period = p_synchro->i_backward_period;
        p_synchro->i_backward_period = i_repeat_field;
        if( p_synchro->backward_pts )
        {
            if( next_dts &&
                (next_dts - p_synchro->backward_pts
                    > PTS_THRESHOLD
                  || p_synchro->backward_pts - next_dts
                    > PTS_THRESHOLD) && !p_synchro->b_quiet )
            {
                msg_Warn( p_synchro->p_dec, "backward_pts != dts (%"PRId64")",
                           next_dts
                               - p_synchro->backward_pts );
            }
            if( (p_synchro->backward_pts - p_synchro->current_pts
                    > PTS_THRESHOLD
                  || p_synchro->current_pts - p_synchro->backward_pts
                    > PTS_THRESHOLD) && !p_synchro->b_quiet )
            {
                msg_Warn( p_synchro->p_dec,
                          "backward_pts != current_pts (%"PRId64")",
                          p_synchro->current_pts
                              - p_synchro->backward_pts );
            }
            p_synchro->current_pts = p_synchro->backward_pts;
            p_synchro->backward_pts = 0;
        }
        else if( next_dts )
        {
            if( (next_dts - p_synchro->current_pts
                    > PTS_THRESHOLD
                  || p_synchro->current_pts - next_dts
                    > PTS_THRESHOLD) && !p_synchro->b_quiet )
            {
                msg_Warn( p_synchro->p_dec, "dts != current_pts (%"PRId64")",
                          p_synchro->current_pts
                              - next_dts );
            }
            /* By definition of a DTS. */
            p_synchro->current_pts = next_dts;
            next_dts = 0;
        }
        if( next_pts )
        {
            /* Store the PTS for the next time we have to date an I picture. */
            p_synchro->backward_pts = next_pts;
            next_pts = 0;
        }
    }
#undef PTS_THRESHOLD
#if 0
    /* Removed for incompatibility with slow motion */
    if( p_synchro->current_pts + DEFAULT_PTS_DELAY < now )
    {
        /* We cannot be _that_ late, something must have happened, reinit
         * the dates. */
        if( !p_synchro->b_quiet )
            msg_Warn( p_synchro->p_dec, "PTS << now (%"PRId64"), resetting",
                      now - p_synchro->current_pts - DEFAULT_PTS_DELAY );
        p_synchro->current_pts = now + DEFAULT_PTS_DELAY;
    }
    if( p_synchro->backward_pts
         && p_synchro->backward_pts + DEFAULT_PTS_DELAY < now )
    {
        /* The same. */
        p_synchro->backward_pts = 0;
    }
#endif
    p_synchro->i_pic++;
}
