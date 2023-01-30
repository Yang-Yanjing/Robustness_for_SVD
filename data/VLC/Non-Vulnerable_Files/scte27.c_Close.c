}
static void Close(vlc_object_t *object)
{
    decoder_t *dec = (decoder_t *)object;
    decoder_sys_t *sys = dec->p_sys;
    free(sys->segment_buffer);
    free(sys);
}
