}
static subpicture_t *spu_new_buffer( decoder_t *p_dec,
                                     const subpicture_updater_t *p_updater )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vout_thread_t *p_vout = NULL;
    subpicture_t *p_subpic;
    int i_attempts = 30;
    while( i_attempts-- )
    {
        if( DecoderIsExitRequested( p_dec ) || p_dec->b_error )
            break;
        p_vout = input_resource_HoldVout( p_owner->p_resource );
        if( p_vout )
            break;
        msleep( DECODER_SPU_VOUT_WAIT_DURATION );
    }
    if( !p_vout )
    {
        msg_Warn( p_dec, "no vout found, dropping subpicture" );
        return NULL;
    }
    if( p_owner->p_spu_vout != p_vout )
    {
        p_owner->i_spu_channel = vout_RegisterSubpictureChannel( p_vout );
        p_owner->i_spu_order = 0;
        p_owner->p_spu_vout = p_vout;
    }
    p_subpic = subpicture_New( p_updater );
    if( p_subpic )
    {
        p_subpic->i_channel = p_owner->i_spu_channel;
        p_subpic->i_order = p_owner->i_spu_order++;
        p_subpic->b_subtitle = true;
    }
    vlc_object_release( p_vout );
    return p_subpic;
}
