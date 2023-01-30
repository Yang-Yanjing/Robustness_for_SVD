 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    demux_t        *p_demux = (demux_t*)p_this;
    demux_sys_t    *p_sys = p_demux->p_sys;
#ifdef HAVE_MPC_MPCDEC_H
    if( p_sys->decoder )
    mpc_demux_exit( p_sys->decoder );
#endif
    free( p_sys );
}
