#include "libvlc.h"
fingerprinter_thread_t *fingerprinter_Create( vlc_object_t *p_this )
{
    fingerprinter_thread_t *p_fingerprint;
    p_fingerprint = ( fingerprinter_thread_t * )
            vlc_custom_create( p_this, sizeof( *p_fingerprint ), "fingerprinter" );
    if( !p_fingerprint )
    {
        msg_Err( p_this, "unable to create fingerprinter" );
        return NULL;
    }
    p_fingerprint->p_module = module_need( p_fingerprint, "fingerprinter",
                                           NULL, false );
    if( !p_fingerprint->p_module )
    {
        vlc_object_release( p_fingerprint );
        msg_Err( p_this, "AcoustID fingerprinter not found" );
        return NULL;
    }
    return p_fingerprint;
}
