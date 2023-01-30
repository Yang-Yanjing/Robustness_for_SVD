 *****************************************************************************/
void EncoderClose( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t*)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    if( !p_sys ) return;
    if( p_sys->p_dmo ) p_sys->p_dmo->vt->Release( (IUnknown *)p_sys->p_dmo );
    FreeLibrary( p_sys->hmsdmo_dll );
    /* Uninitialize OLE/COM */
    CoUninitialize();
    free( p_sys );
}
