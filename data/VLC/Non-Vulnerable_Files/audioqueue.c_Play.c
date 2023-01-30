#pragma mark actual playback
static void Play(audio_output_t *p_aout, block_t *p_block)
{
    AudioQueueBufferRef inBuffer = NULL;
    OSStatus status;
    status = AudioQueueAllocateBuffer(p_aout->sys->audioQueueRef, p_block->i_buffer, &inBuffer);
    if (status == noErr) {
        memcpy(inBuffer->mAudioData, p_block->p_buffer, p_block->i_buffer);
        inBuffer->mAudioDataByteSize = p_block->i_buffer;
        status = AudioQueueEnqueueBuffer(p_aout->sys->audioQueueRef, inBuffer, 0, NULL);
        if (status == noErr)
            p_aout->sys->i_played_length += p_block->i_length;
        else
            msg_Err(p_aout, "enqueuing buffer failed (%li)", status);
    } else
            msg_Err(p_aout, "buffer alloction failed (%li)", status);
    block_Release(p_block);
}
