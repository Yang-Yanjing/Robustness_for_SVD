}
static void Deinit(vlc_va_t *va)
{
    vlc_va_sys_t *sys = va->sys;
    assert(sys->context->decoder != VDP_INVALID_HANDLE);
    vdp_decoder_destroy(sys->vdp, sys->context->decoder);
    for (unsigned i = 0; sys->pool[i] != NULL; i++)
        sys->pool[i]->destroy(sys->pool[i]);
    free(sys->pool);
}
