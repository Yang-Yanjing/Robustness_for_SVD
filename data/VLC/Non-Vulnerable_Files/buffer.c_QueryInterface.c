#include "dmo.h"
static long STDCALL QueryInterface( IUnknown *This,
                                    const GUID *riid, void **ppv )
{
    CMediaBuffer *p_mb = (CMediaBuffer *)This;
    if( !memcmp( riid, &IID_IUnknown, sizeof(GUID) ) ||
        !memcmp( riid, &IID_IMediaBuffer, sizeof(GUID) ) )
    {
        p_mb->i_ref++;
        *ppv = (void *)This;
        return NOERROR;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}
