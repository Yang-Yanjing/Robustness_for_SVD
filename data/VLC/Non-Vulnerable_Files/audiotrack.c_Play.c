}
static void Play(audio_output_t* p_aout, block_t* p_buffer)
{
    aout_sys_t *p_sys = p_aout->sys;
    while (p_buffer->i_buffer) {
        int ret = p_sys->at_write(p_sys->AudioTrack, p_buffer->p_buffer, p_buffer->i_buffer);
        if (ret < 0) {
            msg_Err(p_aout, "Write failed (error %d)", ret);
            break;
        }
        p_sys->samples_written += ret / p_sys->bytes_per_frame;
        p_buffer->p_buffer += ret;
        p_buffer->i_buffer -= ret;
    }
    block_Release( p_buffer );
}
