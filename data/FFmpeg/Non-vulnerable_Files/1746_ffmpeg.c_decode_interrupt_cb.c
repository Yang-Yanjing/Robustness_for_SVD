static int decode_interrupt_cb(void *ctx)
{
    return received_nb_signals > atomic_load(&transcode_init_done);
}
