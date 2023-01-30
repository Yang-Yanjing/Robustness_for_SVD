}
static void Raw1394EventThreadCleanup( void *obj )
{
    event_thread_t *p_ev = (event_thread_t *)obj;
    AVCStop( p_ev->p_access, p_ev->p_access->p_sys->i_node );
}
