 *****************************************************************************/
void FrontendPoll( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    frontend_t * p_frontend = p_sys->p_frontend;
    struct dvb_frontend_event event;
    fe_status_t i_status, i_diff;
    for( ;; )
    {
        if( ioctl( p_sys->i_frontend_handle, FE_GET_EVENT, &event ) < 0 )
        {
            if( errno != EWOULDBLOCK )
                msg_Err( p_access, "frontend event error: %s",
                         vlc_strerror_c(errno) );
            return;
        }
        i_status = event.status;
        i_diff = i_status ^ p_frontend->i_last_status;
        p_frontend->i_last_status = i_status;
        {
#define IF_UP( x )                                                          \
        }                                                                   \
        if ( i_diff & (x) )                                                 \
        {                                                                   \
            if ( i_status & (x) )
            IF_UP( FE_HAS_SIGNAL )
                msg_Dbg( p_access, "frontend has acquired signal" );
            else
                msg_Dbg( p_access, "frontend has lost signal" );
            IF_UP( FE_HAS_CARRIER )
                msg_Dbg( p_access, "frontend has acquired carrier" );
            else
                msg_Dbg( p_access, "frontend has lost carrier" );
            IF_UP( FE_HAS_VITERBI )
                msg_Dbg( p_access, "frontend has acquired stable FEC" );
            else
                msg_Dbg( p_access, "frontend has lost FEC" );
            IF_UP( FE_HAS_SYNC )
                msg_Dbg( p_access, "frontend has acquired sync" );
            else
                msg_Dbg( p_access, "frontend has lost sync" );
            IF_UP( FE_HAS_LOCK )
            {
                frontend_statistic_t stat;
                msg_Dbg( p_access, "frontend has acquired lock" );
                p_sys->i_frontend_timeout = 0;
                /* Read some statistics */
                if( !FrontendGetStatistic( p_access, &stat ) )
                {
                    if( stat.i_ber >= 0 )
                        msg_Dbg( p_access, "- Bit error rate: %d", stat.i_ber );
                    if( stat.i_signal_strenth >= 0 )
                        msg_Dbg( p_access, "- Signal strength: %d", stat.i_signal_strenth );
                    if( stat.i_snr >= 0 )
                        msg_Dbg( p_access, "- SNR: %d", stat.i_snr );
                }
            }
            else
            {
                msg_Dbg( p_access, "frontend has lost lock" );
                p_sys->i_frontend_timeout = mdate() + FRONTEND_LOCK_TIMEOUT;
            }
            IF_UP( FE_REINIT )
            {
                /* The frontend was reinited. */
                msg_Warn( p_access, "reiniting frontend");
                FrontendSet( p_access );
            }
        }
#undef IF_UP
    }
}
