}
static bool postConnectHandler( freerdp *p_instance )
{
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_instance->context;
    msg_Dbg( p_vlccontext->p_demux, "connected to desktop %dx%d (%d bpp)",
#if defined(FREERDP_VERSION_MAJOR) && (FREERDP_VERSION_MAJOR > 1 || (FREERDP_VERSION_MAJOR == 1 && FREERDP_VERSION_MINOR >= 1))
             p_instance->settings->DesktopWidth,
             p_instance->settings->DesktopHeight,
             p_instance->settings->ColorDepth
#else
             p_instance->settings->width,
             p_instance->settings->height,
             p_instance->settings->color_depth
#endif
             );
    p_instance->update->DesktopResize = desktopResizeHandler;
    p_instance->update->BeginPaint = beginPaintHandler;
    p_instance->update->EndPaint = endPaintHandler;
    gdi_init( p_instance,
                CLRBUF_16BPP |
#if defined(FREERDP_VERSION_MAJOR) && defined(FREERDP_VERSION_MINOR) && \
    !(FREERDP_VERSION_MAJOR > 1 || (FREERDP_VERSION_MAJOR == 1 && FREERDP_VERSION_MINOR >= 2))
                CLRBUF_24BPP |
#endif
                CLRBUF_32BPP, NULL );
    desktopResizeHandler( p_instance->context );
    return true;
}
