 *****************************************************************************/
void input_clock_ChangeRate( input_clock_t *cl, int i_rate )
{
    vlc_mutex_lock( &cl->lock );
    if( cl->b_has_reference )
    {
        /* Move the reference point (as if we were playing at the new rate
         * from the start */
        cl->ref.i_system = cl->last.i_system - (cl->last.i_system - cl->ref.i_system) * i_rate / cl->i_rate;
    }
    cl->i_rate = i_rate;
    vlc_mutex_unlock( &cl->lock );
}
