 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    size_t     i_index;
    /* remove the decoder from the global list */
    vlc_mutex_lock( &kate_decoder_list_mutex );
    for( i_index = 0; i_index < kate_decoder_list_size; i_index++ )
    {
        if( kate_decoder_list[ i_index ] == p_dec )
        {
            kate_decoder_list[ i_index ] = kate_decoder_list[ --kate_decoder_list_size ];
            break;
        }
    }
    vlc_mutex_unlock( &kate_decoder_list_mutex );
    msg_Dbg( p_dec, "Closing Kate decoder" );
    DecSysRelease( p_dec->p_sys );
}
