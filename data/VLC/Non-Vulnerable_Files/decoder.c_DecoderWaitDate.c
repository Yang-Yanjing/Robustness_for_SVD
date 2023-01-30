 */
static void DecoderWaitDate( decoder_t *p_dec,
                             bool *pb_reject, mtime_t i_deadline )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    vlc_assert_locked( &p_owner->lock );
    if( *pb_reject || i_deadline < 0 )
        return;
    do
    {
        if( p_owner->b_flushing || p_owner->b_exit )
        {
            *pb_reject = true;
            break;
        }
    }
    while( vlc_cond_timedwait( &p_owner->wait_request, &p_owner->lock,
                               i_deadline ) == 0 );
}
