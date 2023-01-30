 */
static void DirectXGetDDrawCaps(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    /* This is just an indication of whether or not we'll support overlay,
     * but with this test we don't know if we support YUV overlay */
    DDCAPS ddcaps;
    ZeroMemory(&ddcaps, sizeof(ddcaps));
    ddcaps.dwSize = sizeof(ddcaps);
    HRESULT hr = IDirectDraw2_GetCaps(sys->ddobject, &ddcaps, NULL);
    if (hr != DD_OK) {
        msg_Warn(vd, "cannot get caps");
        return;
    }
    /* Determine if the hardware supports overlay surfaces */
    const bool has_overlay = ddcaps.dwCaps & DDCAPS_OVERLAY;
    /* Determine if the hardware supports overlay surfaces */
    const bool has_overlay_fourcc = ddcaps.dwCaps & DDCAPS_OVERLAYFOURCC;
    /* Determine if the hardware supports overlay deinterlacing */
    const bool can_deinterlace = ddcaps.dwCaps & DDCAPS2_CANFLIPODDEVEN;
    /* Determine if the hardware supports colorkeying */
    const bool has_color_key = ddcaps.dwCaps & DDCAPS_COLORKEY;
    /* Determine if the hardware supports scaling of the overlay surface */
    const bool can_stretch = ddcaps.dwCaps & DDCAPS_OVERLAYSTRETCH;
    /* Determine if the hardware supports color conversion during a blit */
    sys->can_blit_fourcc = ddcaps.dwCaps & DDCAPS_BLTFOURCC;
    /* Determine overlay source boundary alignment */
    const bool align_boundary_src  = ddcaps.dwCaps & DDCAPS_ALIGNBOUNDARYSRC;
    /* Determine overlay destination boundary alignment */
    const bool align_boundary_dest = ddcaps.dwCaps & DDCAPS_ALIGNBOUNDARYDEST;
    /* Determine overlay destination size alignment */
    const bool align_size_src  = ddcaps.dwCaps & DDCAPS_ALIGNSIZESRC;
    /* Determine overlay destination size alignment */
    const bool align_size_dest = ddcaps.dwCaps & DDCAPS_ALIGNSIZEDEST;
    msg_Dbg(vd, "DirectDraw Capabilities: overlay=%i yuvoverlay=%i "
                "can_deinterlace_overlay=%i colorkey=%i stretch=%i "
                "bltfourcc=%i",
                has_overlay, has_overlay_fourcc, can_deinterlace,
                has_color_key, can_stretch, sys->can_blit_fourcc);
    if (align_boundary_src || align_boundary_dest || align_size_src || align_size_dest) {
        if (align_boundary_src)
            vd->sys->i_align_src_boundary = ddcaps.dwAlignBoundarySrc;
        if (align_boundary_dest)
            vd->sys->i_align_dest_boundary = ddcaps.dwAlignBoundaryDest;
        if (align_size_src)
            vd->sys->i_align_src_size = ddcaps.dwAlignSizeSrc;
        if (align_size_dest)
            vd->sys->i_align_dest_size = ddcaps.dwAlignSizeDest;
        msg_Dbg(vd,
                "align_boundary_src=%i,%i align_boundary_dest=%i,%i "
                "align_size_src=%i,%i align_size_dest=%i,%i",
                align_boundary_src,  vd->sys->i_align_src_boundary,
                align_boundary_dest, vd->sys->i_align_dest_boundary,
                align_size_src,  vd->sys->i_align_src_size,
                align_size_dest, vd->sys->i_align_dest_size);
    }
}
