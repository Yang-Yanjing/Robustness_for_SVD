}
libvlc_log_iterator_t *libvlc_log_get_iterator( const libvlc_log_t *p_log )
{
    return (p_log != NULL) ? malloc(1) : NULL;
}
