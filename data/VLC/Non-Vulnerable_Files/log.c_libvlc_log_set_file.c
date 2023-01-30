}
void libvlc_log_set_file (libvlc_instance_t *inst, FILE *stream)
{
    libvlc_log_set (inst, libvlc_log_file, stream);
}
