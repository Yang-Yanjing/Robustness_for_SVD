}
static void CloseEncoder(vlc_object_t *p_this)
{
    encoder_t *enc = (encoder_t *)p_this;
    encoder_sys_t *sys = enc->p_sys;
    opus_multistream_encoder_destroy(sys->enc);
    if (sys->padding)
        block_Release(sys->padding);
    free(sys->buffer);
    free(sys);
}
