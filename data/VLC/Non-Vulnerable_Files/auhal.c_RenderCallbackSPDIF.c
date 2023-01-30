 */
static OSStatus RenderCallbackSPDIF(AudioDeviceID inDevice,
                                    const AudioTimeStamp * inNow,
                                    const void * inInputData,
                                    const AudioTimeStamp * inInputTime,
                                    AudioBufferList * outOutputData,
                                    const AudioTimeStamp * inOutputTime,
                                    void * threadGlobals)
{
    VLC_UNUSED(inNow);
    VLC_UNUSED(inDevice);
    VLC_UNUSED(inInputData);
    VLC_UNUSED(inInputTime);
    VLC_UNUSED(inOutputTime);
    audio_output_t * p_aout = (audio_output_t *)threadGlobals;
    struct aout_sys_t * p_sys = p_aout->sys;
    int bytesRequested = outOutputData->mBuffers[p_sys->i_stream_index].mDataByteSize;
    char *targetBuffer = outOutputData->mBuffers[p_sys->i_stream_index].mData;
    if (unlikely(bytesRequested == 0)) /* cannot be negative */
        return noErr;
    vlc_mutex_lock(&p_sys->lock);
    /* Pull audio from buffer */
    int32_t availableBytes;
    char *buffer = TPCircularBufferTail(&p_sys->circular_buffer, &availableBytes);
    /* check if we have enough data */
    if (!availableBytes || p_sys->b_paused) {
        /* return an empty buffer so silence is played until we have data */
        memset(targetBuffer, 0, bytesRequested);
    } else {
        int32_t bytesToCopy = __MIN(bytesRequested, availableBytes);
        memcpy(targetBuffer, buffer, bytesToCopy);
        TPCircularBufferConsume(&p_sys->circular_buffer, bytesToCopy);
        outOutputData->mBuffers[p_sys->i_stream_index].mDataByteSize = bytesToCopy;
    }
    vlc_cond_signal(&p_sys->cond);
    vlc_mutex_unlock(&p_sys->lock);
    return noErr;
}
