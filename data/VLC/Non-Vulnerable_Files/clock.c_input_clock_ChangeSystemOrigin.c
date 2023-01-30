}
void input_clock_ChangeSystemOrigin( input_clock_t *cl, bool b_absolute, mtime_t i_system )
{
    vlc_mutex_lock( &cl->lock );
    assert( cl->b_has_reference );
    mtime_t i_offset;
    if( b_absolute )
    {
        i_offset = i_system - cl->ref.i_system - ClockGetTsOffset( cl );
    }
    else
    {
        if( !cl->b_has_external_clock )
        {
            cl->b_has_external_clock = true;
            cl->i_external_clock     = i_system;
        }
        i_offset = i_system - cl->i_external_clock;
    }
    cl->ref.i_system += i_offset;
    cl->last.i_system += i_offset;
    vlc_mutex_unlock( &cl->lock );
}
