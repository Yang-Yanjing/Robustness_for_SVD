 */
static void Play (audio_output_t *aout, block_t *block)
{
    aout_sys_t *sys = aout->sys;
    if (sys->chans_to_reorder != 0)
        aout_ChannelReorder(block->p_buffer, block->i_buffer,
                           sys->chans_to_reorder, sys->chans_table, sys->format);
    snd_pcm_t *pcm = sys->pcm;
    /* TODO: better overflow handling */
    /* TODO: no period wake ups */
    while (block->i_nb_samples > 0)
    {
        snd_pcm_sframes_t frames;
        frames = snd_pcm_writei (pcm, block->p_buffer, block->i_nb_samples);
        if (frames >= 0)
        {
            size_t bytes = snd_pcm_frames_to_bytes (pcm, frames);
            block->i_nb_samples -= frames;
            block->p_buffer += bytes;
            block->i_buffer -= bytes;
            // pts, length
        }
        else  
        {
            int val = snd_pcm_recover (pcm, frames, 1);
            if (val)
            {
                msg_Err (aout, "cannot recover playback stream: %s",
                         snd_strerror (val));
                DumpDeviceStatus (aout, pcm);
                break;
            }
            msg_Warn (aout, "cannot write samples: %s", snd_strerror (frames));
        }
    }
    block_Release (block);
}
