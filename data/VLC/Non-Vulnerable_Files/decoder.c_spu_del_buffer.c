}
static void spu_del_buffer( decoder_t *p_dec, subpicture_t *p_subpic )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vout_thread_t *p_vout = NULL;
    p_vout = input_resource_HoldVout( p_owner->p_resource );
    if( !p_vout || p_owner->p_spu_vout != p_vout )
    {
        if( p_vout )
            vlc_object_release( p_vout );
        msg_Warn( p_dec, "no vout found, leaking subpicture" );
        return;
    }
    subpicture_Delete( p_subpic );
    vlc_object_release( p_vout );
}
