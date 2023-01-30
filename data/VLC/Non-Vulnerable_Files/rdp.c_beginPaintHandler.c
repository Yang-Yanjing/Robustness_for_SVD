}
static void beginPaintHandler( rdpContext *p_context )
{
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_context;
    demux_sys_t *p_sys = p_vlccontext->p_demux->p_sys;
    rdpGdi *p_gdi = p_context->gdi;
    p_gdi->primary->hdc->hwnd->invalid->null = 1;
    p_gdi->primary->hdc->hwnd->ninvalid = 0;
    if ( ! p_sys->p_block && p_sys->i_framebuffersize )
        p_sys->p_block = block_Alloc( p_sys->i_framebuffersize );
}
