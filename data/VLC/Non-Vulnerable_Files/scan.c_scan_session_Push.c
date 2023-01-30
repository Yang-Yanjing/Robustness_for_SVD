}
bool scan_session_Push( scan_session_t *p_scan, block_t *p_block )
{
    if( p_block->i_buffer < 188 || p_block->p_buffer[0] != 0x47 )
    {
        block_Release( p_block );
        return false;
    }
    /* */
    const int i_pid = ( (p_block->p_buffer[1]&0x1f)<<8) | p_block->p_buffer[2];
    if( i_pid == 0x00 )
    {
        if( !p_scan->pat )
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        {
            p_scan->pat = dvbpsi_new( &dvbpsi_messages, DVBPSI_MSG_DEBUG );
            if( !p_scan->pat )
            {
                block_Release( p_block );
                return false;
            }
            p_scan->pat->p_sys = (void *) VLC_OBJECT(p_scan->p_obj);
            if( !dvbpsi_pat_attach( p_scan->pat, (dvbpsi_pat_callback)PATCallBack, p_scan ) )
            {
                dvbpsi_delete( p_scan->pat );
                p_scan->pat = NULL;
                block_Release( p_block );
                return false;
            }
        }
#else
            p_scan->pat = dvbpsi_AttachPAT( (dvbpsi_pat_callback)PATCallBack, p_scan );
#endif
        if( p_scan->pat )
            dvbpsi_PushPacket( p_scan->pat, p_block->p_buffer );
    }
    else if( i_pid == 0x11 )
    {
        if( !p_scan->sdt )
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        {
            p_scan->sdt = dvbpsi_new( &dvbpsi_messages, DVBPSI_MSG_DEBUG );
            if( !p_scan->sdt )
            {
                block_Release( p_block );
                return false;
            }
            p_scan->sdt->p_sys = (void *) VLC_OBJECT(p_scan->p_obj);
            if( !dvbpsi_AttachDemux( p_scan->sdt, (dvbpsi_demux_new_cb_t)PSINewTableCallBack, p_scan ) )
            {
                dvbpsi_delete( p_scan->sdt );
                p_scan->sdt = NULL;
                block_Release( p_block );
                return false;
            }
        }
#else
            p_scan->sdt = dvbpsi_AttachDemux( (dvbpsi_demux_new_cb_t)PSINewTableCallBack, p_scan );
#endif
        if( p_scan->sdt )
            dvbpsi_PushPacket( p_scan->sdt, p_block->p_buffer );
    }
    else /*if( i_pid == p_scan->i_nit_pid )*/
    {
#ifdef DVBPSI_USE_NIT
        if( !p_scan->nit )
# if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        {
            p_scan->nit = dvbpsi_new( &dvbpsi_messages, DVBPSI_MSG_DEBUG );
            if( !p_scan->nit )
            {
                block_Release( p_block );
                return false;
            }
            p_scan->nit->p_sys = (void *) VLC_OBJECT(p_scan->p_obj);
            if( !dvbpsi_AttachDemux( p_scan->nit, (dvbpsi_demux_new_cb_t)PSINewTableCallBack, p_scan ) )
            {
                dvbpsi_delete( p_scan->nit );
                p_scan->nit = NULL;
                block_Release( p_block );
                return false;
            }
        }
# else
            p_scan->nit = dvbpsi_AttachDemux( (dvbpsi_demux_new_cb_t)PSINewTableCallBack, p_scan );
# endif
        if( p_scan->nit )
            dvbpsi_PushPacket( p_scan->nit, p_block->p_buffer );
#endif
    }
    block_Release( p_block );
    return p_scan->p_pat && p_scan->p_sdt && 
#ifdef DVBPSI_USE_NIT
        p_scan->p_nit;
#else
        true;
#endif
}
