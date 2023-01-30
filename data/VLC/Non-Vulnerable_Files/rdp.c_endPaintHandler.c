}
static void endPaintHandler( rdpContext *p_context )
{
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_context;
    demux_sys_t *p_sys = p_vlccontext->p_demux->p_sys;
    rdpGdi *p_gdi = p_context->gdi;
    if ( p_sys->p_block )
    {
        p_sys->p_block->i_buffer = p_sys->i_framebuffersize;
        memcpy( p_sys->p_block->p_buffer, p_gdi->primary_buffer, p_sys->p_block->i_buffer );
    }
}
