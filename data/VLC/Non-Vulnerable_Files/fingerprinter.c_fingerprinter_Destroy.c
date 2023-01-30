}
void fingerprinter_Destroy( fingerprinter_thread_t *p_fingerprint )
{
    module_unneed( p_fingerprint, p_fingerprint->p_module );
    vlc_object_release( p_fingerprint );
}
