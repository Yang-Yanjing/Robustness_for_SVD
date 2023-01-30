#undef sout_EncoderCreate
encoder_t *sout_EncoderCreate( vlc_object_t *p_this )
{
    msg_Err (p_this, "Encoding support not compiled-in!");
    return NULL;
}
