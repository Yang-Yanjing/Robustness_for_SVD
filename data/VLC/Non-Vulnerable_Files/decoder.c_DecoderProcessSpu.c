 */
static void DecoderProcessSpu( decoder_t *p_dec, block_t *p_block, bool b_flush )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    input_thread_t *p_input = p_owner->p_input;
    vout_thread_t *p_vout;
    subpicture_t *p_spu;
    while( (p_spu = p_dec->pf_decode_sub( p_dec, p_block ? &p_block : NULL ) ) )
    {
        if( p_input != NULL )
        {
            vlc_mutex_lock( &p_input->p->counters.counters_lock );
            stats_Update( p_input->p->counters.p_decoded_sub, 1, NULL );
            vlc_mutex_unlock( &p_input->p->counters.counters_lock );
        }
        p_vout = input_resource_HoldVout( p_owner->p_resource );
        if( p_vout && p_owner->p_spu_vout == p_vout )
        {
            /* Preroll does not work very well with subtitle */
            if( p_spu->i_start > VLC_TS_INVALID &&
                p_spu->i_start < p_owner->i_preroll_end &&
                ( p_spu->i_stop <= VLC_TS_INVALID || p_spu->i_stop < p_owner->i_preroll_end ) )
            {
                subpicture_Delete( p_spu );
            }
            else
            {
                DecoderPlaySpu( p_dec, p_spu );
            }
        }
        else
        {
            subpicture_Delete( p_spu );
        }
        if( p_vout )
            vlc_object_release( p_vout );
    }
    if( b_flush && p_owner->p_spu_vout )
    {
        p_vout = input_resource_HoldVout( p_owner->p_resource );
        if( p_vout && p_owner->p_spu_vout == p_vout )
            vout_FlushSubpictureChannel( p_vout, p_owner->i_spu_channel );
        if( p_vout )
            vlc_object_release( p_vout );
    }
}
