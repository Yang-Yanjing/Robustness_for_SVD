 *****************************************************************************/
static void *DemuxThread( void *p_data )
{
    demux_t *p_demux = (demux_t *) p_data;
    demux_sys_t *p_sys = p_demux->p_sys;
    p_sys->i_starttime = mdate();
    mtime_t i_next_frame_date = mdate() + p_sys->i_frame_interval;
    int i_ret;
    for(;;)
    {
        i_ret = 0;
        p_sys->i_cancel_state = vlc_savecancel();
        if ( freerdp_shall_disconnect( p_sys->p_instance ) )
        {
            vlc_restorecancel( p_sys->i_cancel_state );
            msg_Warn( p_demux, "RDP server closed session" );
            es_out_Del( p_demux->out, p_sys->es );
            p_sys->es = NULL;
            return NULL;
        }
        struct
        {
            void* pp_rfds[RDP_MAX_FD]; /* Declared by rdp */
            void* pp_wfds[RDP_MAX_FD];
            int i_nbr;
            int i_nbw;
            struct pollfd ufds[RDP_MAX_FD];
        } fds;
        fds.i_nbr = fds.i_nbw = 0;
        if ( freerdp_get_fds( p_sys->p_instance, fds.pp_rfds, &fds.i_nbr,
                              fds.pp_wfds, &fds.i_nbw ) != true )
        {
            vlc_restorecancel( p_sys->i_cancel_state );
            msg_Err( p_demux, "cannot get FDS" );
        }
        else
        if ( (fds.i_nbr + fds.i_nbw) > 0 && p_sys->es )
        {
            vlc_restorecancel( p_sys->i_cancel_state );
            int i_count = 0;
            for( int i = 0; i < fds.i_nbr; i++ )
            {
                fds.ufds[ i_count ].fd = (long) fds.pp_rfds[ i ];
                fds.ufds[ i_count ].events = POLLIN ;
                fds.ufds[ i_count++ ].revents = 0;
            }
            for( int i = 0; i < fds.i_nbw && i_count < RDP_MAX_FD; i++ )
            {   /* may be useless */
                fds.ufds[ i_count ].fd = (long) fds.pp_wfds[ i ];
                fds.ufds[ i_count ].events = POLLOUT;
                fds.ufds[ i_count++ ].revents = 0;
            }
            i_ret = poll( fds.ufds, i_count, p_sys->i_frame_interval * 1000/2 );
        } else {
            vlc_restorecancel( p_sys->i_cancel_state );
        }
        mwait( i_next_frame_date );
        i_next_frame_date += p_sys->i_frame_interval;
        if ( i_ret >= 0 )
        {
            /* Do the rendering */
            p_sys->i_cancel_state = vlc_savecancel();
            freerdp_check_fds( p_sys->p_instance );
            vlc_restorecancel( p_sys->i_cancel_state );
            block_t *p_block = block_Duplicate( p_sys->p_block );
            if (likely( p_block && p_sys->p_block ))
            {
                p_sys->p_block->i_dts = p_sys->p_block->i_pts = mdate() - p_sys->i_starttime;
                es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_sys->p_block->i_pts );
                es_out_Send( p_demux->out, p_sys->es, p_sys->p_block );
                p_sys->p_block = p_block;
            }
        }
    }
    return NULL;
}
