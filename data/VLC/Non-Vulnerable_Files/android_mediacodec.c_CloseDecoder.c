}
static void CloseDecoder(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    JNIEnv *env = NULL;
    if (!p_sys)
        return;
    /* Invalidate all pictures that are currently in flight in order
     * to prevent the vout from using destroyed output buffers. */
    if (p_sys->direct_rendering)
        InvalidateAllPictures(p_dec);
    jni_attach_thread(&env, THREAD_NAME);
    if (p_sys->input_buffers)
        (*env)->DeleteGlobalRef(env, p_sys->input_buffers);
    if (p_sys->output_buffers)
        (*env)->DeleteGlobalRef(env, p_sys->output_buffers);
    if (p_sys->codec) {
        if (p_sys->started)
        {
            (*env)->CallVoidMethod(env, p_sys->codec, p_sys->stop);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Err(p_dec, "Exception in MediaCodec.stop");
                (*env)->ExceptionClear(env);
            }
        }
        if (p_sys->allocated)
        {
            (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Err(p_dec, "Exception in MediaCodec.release");
                (*env)->ExceptionClear(env);
            }
        }
        (*env)->DeleteGlobalRef(env, p_sys->codec);
    }
    if (p_sys->buffer_info)
        (*env)->DeleteGlobalRef(env, p_sys->buffer_info);
    jni_detach_thread();
    free(p_sys->name);
    ArchitectureSpecificCopyHooksDestroy(p_sys->pixel_format, &p_sys->architecture_specific_data);
    free(p_sys->inflight_picture);
    if (p_sys->timestamp_fifo)
        timestamp_FifoRelease(p_sys->timestamp_fifo);
    free(p_sys);
}
