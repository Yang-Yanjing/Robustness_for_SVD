static mfxStatus frame_free(mfxHDL pthis, mfxFrameAllocResponse *resp)
{
    DecodeContext *decode = pthis;
    if (decode->surfaces)
        vaDestroySurfaces(decode->va_dpy, decode->surfaces, decode->nb_surfaces);
    av_freep(&decode->surfaces);
    av_freep(&decode->surface_ids);
    av_freep(&decode->surface_used);
    decode->nb_surfaces = 0;
    return MFX_ERR_NONE;
}
