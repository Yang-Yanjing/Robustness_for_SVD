}
static void Stop(audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    SetPlayState(sys->playerPlay, SL_PLAYSTATE_STOPPED);
    //Flush remaining buffers if any.
    Clear(sys->playerBufferQueue);
    free(sys->buf);
    block_ChainRelease(sys->p_buffer_chain);
    Destroy(sys->playerObject);
    sys->playerObject = NULL;
}
