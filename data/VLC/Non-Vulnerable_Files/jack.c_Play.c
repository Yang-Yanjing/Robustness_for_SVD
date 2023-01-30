}
static void Play (audio_output_t * p_aout, block_t * p_block)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    jack_ringbuffer_t *rb = p_sys->p_jack_ringbuffer;
    const size_t bytes_per_frame = p_sys->i_channels * sizeof(jack_sample_t);
    while (p_block->i_buffer > 0) {
        /* move data to buffer */
        const size_t write_space = jack_ringbuffer_write_space(rb);
        const size_t bytes = p_block->i_buffer < write_space ?
            p_block->i_buffer : write_space;
        /* If our audio thread is not reading fast enough */
        if( unlikely( bytes == 0 ) ) {
            msg_Warn( p_aout, "%"PRIuPTR " frames of audio dropped",
                    p_block->i_buffer /  bytes_per_frame );
            break;
        }
        jack_ringbuffer_write( rb, (const char *) p_block->p_buffer, bytes );
        p_block->p_buffer += bytes;
        p_block->i_buffer -= bytes;
    }
    block_Release(p_block);
}
