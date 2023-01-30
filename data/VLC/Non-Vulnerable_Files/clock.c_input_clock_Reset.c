 *****************************************************************************/
void input_clock_Reset( input_clock_t *cl )
{
    vlc_mutex_lock( &cl->lock );
    cl->b_has_reference = false;
    cl->ref = clock_point_Create( VLC_TS_INVALID, VLC_TS_INVALID );
    cl->b_has_external_clock = false;
    cl->i_ts_max = VLC_TS_INVALID;
    vlc_mutex_unlock( &cl->lock );
}
