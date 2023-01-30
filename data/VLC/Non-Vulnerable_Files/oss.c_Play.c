 */
static void Play (audio_output_t *aout, block_t *block)
{
    aout_sys_t *sys = aout->sys;
    int fd = sys->fd;
    while (block->i_buffer > 0)
    {
        ssize_t bytes = write (fd, block->p_buffer, block->i_buffer);
        if (bytes >= 0)
        {
            block->p_buffer += bytes;
            block->i_buffer -= bytes;
        }
        else
            msg_Err (aout, "cannot write samples: %s", vlc_strerror_c(errno));
    }
    block_Release (block);
}
