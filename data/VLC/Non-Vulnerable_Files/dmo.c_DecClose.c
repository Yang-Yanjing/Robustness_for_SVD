}
static void DecClose( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->p_dmo ) p_sys->p_dmo->vt->Release( (IUnknown *)p_sys->p_dmo );
    FreeLibrary( p_sys->hmsdmo_dll );
    CoUninitialize();
    free( p_sys->p_buffer );
}
