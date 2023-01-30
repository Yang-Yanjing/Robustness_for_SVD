static mfxStatus frame_alloc(mfxHDL pthis, mfxFrameAllocRequest *req,
                             mfxFrameAllocResponse *resp)
{
    DecodeContext *decode = pthis;
    int err, i;
    if (decode->surfaces) {
        fprintf(stderr, "Multiple allocation requests.\n");
        return MFX_ERR_MEMORY_ALLOC;
    }
    if (!(req->Type & MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET)) {
        fprintf(stderr, "Unsupported surface type: %d\n", req->Type);
        return MFX_ERR_UNSUPPORTED;
    }
    if (req->Info.BitDepthLuma != 8 || req->Info.BitDepthChroma != 8 ||
        req->Info.Shift || req->Info.FourCC != MFX_FOURCC_NV12 ||
        req->Info.ChromaFormat != MFX_CHROMAFORMAT_YUV420) {
        fprintf(stderr, "Unsupported surface properties.\n");
        return MFX_ERR_UNSUPPORTED;
    }
    decode->surfaces     = av_malloc_array (req->NumFrameSuggested, sizeof(*decode->surfaces));
    decode->surface_ids  = av_malloc_array (req->NumFrameSuggested, sizeof(*decode->surface_ids));
    decode->surface_used = av_mallocz_array(req->NumFrameSuggested, sizeof(*decode->surface_used));
    if (!decode->surfaces || !decode->surface_ids || !decode->surface_used)
        goto fail;
    err = vaCreateSurfaces(decode->va_dpy, VA_RT_FORMAT_YUV420,
                           req->Info.Width, req->Info.Height,
                           decode->surfaces, req->NumFrameSuggested,
                           NULL, 0);
    if (err != VA_STATUS_SUCCESS) {
        fprintf(stderr, "Error allocating VA surfaces\n");
        goto fail;
    }
    decode->nb_surfaces = req->NumFrameSuggested;
    for (i = 0; i < decode->nb_surfaces; i++)
        decode->surface_ids[i] = &decode->surfaces[i];
    resp->mids           = decode->surface_ids;
    resp->NumFrameActual = decode->nb_surfaces;
    decode->frame_info = req->Info;
    return MFX_ERR_NONE;
fail:
    av_freep(&decode->surfaces);
    av_freep(&decode->surface_ids);
    av_freep(&decode->surface_used);
    return MFX_ERR_MEMORY_ALLOC;
}
