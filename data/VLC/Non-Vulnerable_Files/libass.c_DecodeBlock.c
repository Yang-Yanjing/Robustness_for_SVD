 ****************************************************************************/
static subpicture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t *p_spu = NULL;
    block_t *p_block;
    if( !pp_block || *pp_block == NULL )
        return NULL;
    p_block = *pp_block;
    if( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        p_sys->i_max_stop = VLC_TS_INVALID;
        block_Release( p_block );
        return NULL;
    }
    *pp_block = NULL;
    if( p_block->i_buffer == 0 || p_block->p_buffer[0] == '\0' )
    {
        block_Release( p_block );
        return NULL;
    }
    subpicture_updater_sys_t *p_spu_sys = malloc( sizeof(*p_spu_sys) );
    if( !p_spu_sys )
    {
        block_Release( p_block );
        return NULL;
    }
    subpicture_updater_t updater = {
        .pf_validate = SubpictureValidate,
        .pf_update   = SubpictureUpdate,
        .pf_destroy  = SubpictureDestroy,
        .p_sys       = p_spu_sys,
    };
    p_spu = decoder_NewSubpicture( p_dec, &updater );
    if( !p_spu )
    {
        msg_Warn( p_dec, "can't get spu buffer" );
        free( p_spu_sys );
        block_Release( p_block );
        return NULL;
    }
    p_spu_sys->p_img = NULL;
    p_spu_sys->p_dec_sys = p_sys;
    p_spu_sys->i_subs_len = p_block->i_buffer;
    p_spu_sys->p_subs_data = malloc( p_block->i_buffer );
    p_spu_sys->i_pts = p_block->i_pts;
    if( !p_spu_sys->p_subs_data )
    {
        decoder_DeleteSubpicture( p_dec, p_spu );
        block_Release( p_block );
        return NULL;
    }
    memcpy( p_spu_sys->p_subs_data, p_block->p_buffer,
            p_block->i_buffer );
    p_spu->i_start = p_block->i_pts;
    p_spu->i_stop = __MAX( p_sys->i_max_stop, p_block->i_pts + p_block->i_length );
    p_spu->b_ephemer = true;
    p_spu->b_absolute = true;
    p_sys->i_max_stop = p_spu->i_stop;
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->p_track )
    {
        ass_process_chunk( p_sys->p_track, p_spu_sys->p_subs_data, p_spu_sys->i_subs_len,
                           p_block->i_pts / 1000, p_block->i_length / 1000 );
    }
    vlc_mutex_unlock( &p_sys->lock );
    DecSysHold( p_sys ); /* Keep a reference for the returned subpicture */
    block_Release( p_block );
    return p_spu;
}
