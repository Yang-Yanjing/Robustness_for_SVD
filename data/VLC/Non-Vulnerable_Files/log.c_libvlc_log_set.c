}
void libvlc_log_set (libvlc_instance_t *inst, libvlc_log_cb cb, void *data)
{
    libvlc_log_unset (inst); /* <- Barrier before modifying the callback */
    inst->log.cb = cb;
    inst->log.data = data;
    vlc_LogSet (inst->p_libvlc_int, libvlc_logf, inst);
}
