}
void UnusedAudioQueueCallback(void * inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    /* this function does nothing, but needs to be here to make the AudioQueue API happy.
     * additionally, we clean-up after empty buffers */
    VLC_UNUSED(inUserData);
    AudioQueueFreeBuffer(inAQ, inBuffer);
}
