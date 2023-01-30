 *****************************************************************************/
static subpicture_t *DecodePacket( decoder_t *p_dec, kate_packet *p_kp, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    const kate_event *ev = NULL;
    subpicture_t *p_spu = NULL;
    int i_ret;
    if( !p_sys->b_ready )
    {
        msg_Err( p_dec, "Cannot decode Kate packet, decoder not initialized" );
        return NULL;
    }
    i_ret = kate_decode_packetin( &p_sys->k, p_kp );
    if( i_ret < 0 )
    {
        msg_Err( p_dec, "Kate failed to decode packet: %d", i_ret );
        return NULL;
    }
    i_ret = kate_decode_eventout( &p_sys->k, &ev );
    if( i_ret < 0 )
    {
        msg_Err( p_dec, "Kate failed to retrieve event: %d", i_ret );
        return NULL;
    }
    if( i_ret > 0 )
    {
        /* no event to go with this packet, this is normal */
        return NULL;
    }
    /* we have an event */
    /* Get a new spu */
    subpicture_updater_sys_t *p_spu_sys = NULL;
    if( p_sys->b_use_tiger)
    {
        p_spu_sys = malloc( sizeof(*p_spu_sys) );
        if( !p_spu_sys )
            return NULL;
    }
    subpicture_updater_t updater = {
#ifdef HAVE_TIGER
        .pf_validate = TigerValidateSubpicture,
        .pf_update   = TigerUpdateSubpicture,
        .pf_destroy  = TigerDestroySubpicture,
#endif
        .p_sys       = p_spu_sys,
    };
    p_spu = decoder_NewSubpicture( p_dec, p_sys->b_use_tiger ? &updater : NULL );
    if( !p_spu )
    {
        free( p_spu_sys );
        /* this will happen for lyrics as there is no vout - so no error */
        /* msg_Err( p_dec, "Failed to allocate spu buffer" ); */
        return NULL;
    }
    p_spu->i_start = p_block->i_pts;
    p_spu->i_stop = p_block->i_pts + CLOCK_FREQ *
        ev->duration * p_sys->ki.gps_denominator / p_sys->ki.gps_numerator;
    p_spu->b_ephemer = false;
    p_spu->b_absolute = false;
#ifdef HAVE_TIGER
    if( p_sys->b_use_tiger)
    {
        p_spu_sys->p_dec_sys = p_sys;
        p_spu_sys->i_start   = p_block->i_pts;
        DecSysHold( p_sys );
        p_spu->i_stop = __MAX( p_sys->i_max_stop, p_spu->i_stop );
        p_spu->b_ephemer = true;
        p_spu->b_absolute = true;
        /* add the event to tiger */
        vlc_mutex_lock( &p_sys->lock );
        CHECK_TIGER_RET( tiger_renderer_add_event( p_sys->p_tr, ev->ki, ev ) );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else
#endif
    {
        p_spu = SetupSimpleKateSPU( p_dec, p_spu, ev );
    }
    return p_spu;
}
