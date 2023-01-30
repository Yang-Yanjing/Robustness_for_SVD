}
void input_Join( input_thread_t *p_input )
{
    if( p_input->p->is_running )
        vlc_join( p_input->p->thread, NULL );
}
