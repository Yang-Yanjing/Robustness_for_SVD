};
static void WINAPI DMOFreeMediaType( DMO_MEDIA_TYPE *mt )
{
    if( mt->cbFormat != 0 ) CoTaskMemFree( (PVOID)mt->pbFormat );
    if( mt->pUnk != NULL ) mt->pUnk->vt->Release( (IUnknown *)mt->pUnk );
    mt->cbFormat = 0;
    mt->pbFormat = NULL;
    mt->pUnk = NULL;
}
