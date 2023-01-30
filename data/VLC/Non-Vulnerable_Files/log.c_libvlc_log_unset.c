}
void libvlc_log_unset (libvlc_instance_t *inst)
{
    vlc_LogSet (inst->p_libvlc_int, NULL, NULL);
}
