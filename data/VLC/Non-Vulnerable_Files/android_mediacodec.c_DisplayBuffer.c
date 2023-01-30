 *****************************************************************************/
static void DisplayBuffer(picture_sys_t* p_picsys, bool b_render)
{
    decoder_t *p_dec = p_picsys->p_dec;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if (!p_picsys->b_valid)
        return;
    vlc_mutex_lock(get_android_opaque_mutex());
    /* Picture might have been invalidated while waiting on the mutex. */
    if (!p_picsys->b_valid) {
        vlc_mutex_unlock(get_android_opaque_mutex());
        return;
    }
    uint32_t i_index = p_picsys->i_index;
    p_sys->inflight_picture[i_index] = NULL;
    /* Release the MediaCodec buffer. */
    JNIEnv *env = NULL;
    jni_attach_thread(&env, THREAD_NAME);
    (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release_output_buffer, i_index, b_render);
    if ((*env)->ExceptionOccurred(env)) {
        msg_Err(p_dec, "Exception in MediaCodec.releaseOutputBuffer (DisplayBuffer)");
        (*env)->ExceptionClear(env);
    }
    jni_detach_thread();
    p_picsys->b_valid = false;
    vlc_mutex_unlock(get_android_opaque_mutex());
}
