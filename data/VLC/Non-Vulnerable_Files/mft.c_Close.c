}
void Close(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    MFHandle *mf = &p_sys->mf_handle;
    DestroyMFT(p_dec);
    if (mf->mfplat_dll)
        FreeLibrary(mf->mfplat_dll);
    free(p_sys);
    CoUninitialize();
}
