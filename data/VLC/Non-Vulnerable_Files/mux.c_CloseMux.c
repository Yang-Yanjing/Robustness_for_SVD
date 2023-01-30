 *****************************************************************************/
void CloseMux( vlc_object_t *p_this )
{
    sout_mux_t *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    if( !p_sys->b_write_header && !p_sys->b_error && av_write_trailer( p_sys->oc ) < 0 )
    {
        msg_Err( p_mux, "could not write trailer" );
    }
    avformat_free_context(p_sys->oc);
    av_free(p_sys->io);
    free( p_sys->io_buffer );
    free( p_sys );
}
