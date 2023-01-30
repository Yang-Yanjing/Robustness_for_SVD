 *****************************************************************************/
int input_clock_GetRate( input_clock_t *cl )
{
    int i_rate;
    vlc_mutex_lock( &cl->lock );
    i_rate = cl->i_rate;
    vlc_mutex_unlock( &cl->lock );
    return i_rate;
}
