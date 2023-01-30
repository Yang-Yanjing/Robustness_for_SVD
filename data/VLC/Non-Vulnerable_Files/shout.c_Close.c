 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_access_out_t *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    if( p_sys->p_shout )
    {
        shout_close( p_sys->p_shout );
        shout_free( p_sys->p_shout );
        shout_shutdown();
    }
    free( p_sys );
    msg_Dbg( p_access, "shout access output closed" );
}
