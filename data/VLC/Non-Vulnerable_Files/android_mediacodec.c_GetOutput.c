}
static void GetOutput(decoder_t *p_dec, JNIEnv *env, picture_t **pp_pic, jlong timeout)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    while (1) {
        int index = (*env)->CallIntMethod(env, p_sys->codec, p_sys->dequeue_output_buffer,
                                          p_sys->buffer_info, timeout);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Err(p_dec, "Exception in MediaCodec.dequeueOutputBuffer (GetOutput)");
            (*env)->ExceptionClear(env);
            p_sys->error_state = true;
            return;
        }
        if (index >= 0) {
            if (!p_sys->pixel_format) {
                msg_Warn(p_dec, "Buffers returned before output format is set, dropping frame");
                (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release_output_buffer, index, false);
                if ((*env)->ExceptionOccurred(env)) {
                    msg_Err(p_dec, "Exception in MediaCodec.releaseOutputBuffer");
                    (*env)->ExceptionClear(env);
                    p_sys->error_state = true;
                    return;
                }
                continue;
            }
            if (!*pp_pic) {
                *pp_pic = decoder_NewPicture(p_dec);
            } else if (p_sys->direct_rendering) {
                picture_t *p_pic = *pp_pic;
                picture_sys_t *p_picsys = p_pic->p_sys;
                int i_prev_index = p_picsys->i_index;
                (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release_output_buffer, i_prev_index, false);
                if ((*env)->ExceptionOccurred(env)) {
                    msg_Err(p_dec, "Exception in MediaCodec.releaseOutputBuffer " \
                            "(GetOutput, overwriting previous picture)");
                    (*env)->ExceptionClear(env);
                    p_sys->error_state = true;
                    return;
                }
                // No need to lock here since the previous picture was not sent.
                p_sys->inflight_picture[i_prev_index] = NULL;
            }
            if (*pp_pic) {
                picture_t *p_pic = *pp_pic;
                // TODO: Use crop_top/crop_left as well? Or is that already taken into account?
                // On OMX_TI_COLOR_FormatYUV420PackedSemiPlanar the offset already incldues
                // the cropping, so the top/left cropping params should just be ignored.
                /* If the oldest input block had no PTS, the timestamp
                 * of the frame returned by MediaCodec might be wrong
                 * so we overwrite it with the corresponding dts. */
                int64_t forced_ts = timestamp_FifoGet(p_sys->timestamp_fifo);
                if (forced_ts == VLC_TS_INVALID)
                    p_pic->date = (*env)->GetLongField(env, p_sys->buffer_info, p_sys->pts_field);
                else
                    p_pic->date = forced_ts;
                if (p_sys->direct_rendering) {
                    picture_sys_t *p_picsys = p_pic->p_sys;
                    p_picsys->pf_display_callback = DisplayCallback;
                    p_picsys->pf_unlock_callback = UnlockCallback;
                    p_picsys->p_dec = p_dec;
                    p_picsys->i_index = index;
                    p_picsys->b_valid = true;
                    p_sys->inflight_picture[index] = p_pic;
                } else {
                    jobject buf = (*env)->GetObjectArrayElement(env, p_sys->output_buffers, index);
                    jsize buf_size = (*env)->GetDirectBufferCapacity(env, buf);
                    uint8_t *ptr = (*env)->GetDirectBufferAddress(env, buf);
                    int size = (*env)->GetIntField(env, p_sys->buffer_info, p_sys->size_field);
                    int offset = (*env)->GetIntField(env, p_sys->buffer_info, p_sys->offset_field);
                    ptr += offset; // Check the size parameter as well
                    unsigned int chroma_div;
                    GetVlcChromaSizes(p_dec->fmt_out.i_codec, p_dec->fmt_out.video.i_width,
                                      p_dec->fmt_out.video.i_height, NULL, NULL, &chroma_div);
                    CopyOmxPicture(p_sys->pixel_format, p_pic, p_sys->slice_height, p_sys->stride,
                                   ptr, chroma_div, &p_sys->architecture_specific_data);
                    (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release_output_buffer, index, false);
                    jthrowable exception = (*env)->ExceptionOccurred(env);
                    if (exception != NULL) {
                        jclass illegalStateException = (*env)->FindClass(env, "java/lang/IllegalStateException");
                        if((*env)->IsInstanceOf(env, exception, illegalStateException)) {
                            msg_Err(p_dec, "Codec error (IllegalStateException) in MediaCodec.releaseOutputBuffer");
                            (*env)->ExceptionClear(env);
                            (*env)->DeleteLocalRef(env, illegalStateException);
                            p_sys->error_state = true;
                        }
                    }
                    (*env)->DeleteLocalRef(env, buf);
                }
            } else {
                msg_Warn(p_dec, "NewPicture failed");
                (*env)->CallVoidMethod(env, p_sys->codec, p_sys->release_output_buffer, index, false);
                if ((*env)->ExceptionOccurred(env)) {
                    msg_Err(p_dec, "Exception in MediaCodec.releaseOutputBuffer (GetOutput)");
                    (*env)->ExceptionClear(env);
                    p_sys->error_state = true;
                }
            }
            return;
        } else if (index == INFO_OUTPUT_BUFFERS_CHANGED) {
            msg_Dbg(p_dec, "output buffers changed");
            (*env)->DeleteGlobalRef(env, p_sys->output_buffers);
            p_sys->output_buffers = (*env)->CallObjectMethod(env, p_sys->codec,
                                                             p_sys->get_output_buffers);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Err(p_dec, "Exception in MediaCodec.getOutputBuffer (GetOutput)");
                (*env)->ExceptionClear(env);
                p_sys->output_buffers = NULL;
                p_sys->error_state = true;
                return;
            }
            p_sys->output_buffers = (*env)->NewGlobalRef(env, p_sys->output_buffers);
            vlc_mutex_lock(get_android_opaque_mutex());
            free(p_sys->inflight_picture);
            p_sys->i_output_buffers = (*env)->GetArrayLength(env, p_sys->output_buffers);
            p_sys->inflight_picture = calloc(1, sizeof(picture_t*) * p_sys->i_output_buffers);
            vlc_mutex_unlock(get_android_opaque_mutex());
        } else if (index == INFO_OUTPUT_FORMAT_CHANGED) {
            jobject format = (*env)->CallObjectMethod(env, p_sys->codec, p_sys->get_output_format);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Err(p_dec, "Exception in MediaCodec.getOutputFormat (GetOutput)");
                (*env)->ExceptionClear(env);
                p_sys->error_state = true;
                return;
            }
            jobject format_string = (*env)->CallObjectMethod(env, format, p_sys->tostring);
            jsize format_len = (*env)->GetStringUTFLength(env, format_string);
            const char *format_ptr = (*env)->GetStringUTFChars(env, format_string, NULL);
            msg_Dbg(p_dec, "output format changed: %.*s", format_len, format_ptr);
            (*env)->ReleaseStringUTFChars(env, format_string, format_ptr);
            ArchitectureSpecificCopyHooksDestroy(p_sys->pixel_format, &p_sys->architecture_specific_data);
            int width           = GET_INTEGER(format, "width");
            int height          = GET_INTEGER(format, "height");
            p_sys->stride       = GET_INTEGER(format, "stride");
            p_sys->slice_height = GET_INTEGER(format, "slice-height");
            p_sys->pixel_format = GET_INTEGER(format, "color-format");
            p_sys->crop_left    = GET_INTEGER(format, "crop-left");
            p_sys->crop_top     = GET_INTEGER(format, "crop-top");
            int crop_right      = GET_INTEGER(format, "crop-right");
            int crop_bottom     = GET_INTEGER(format, "crop-bottom");
            const char *name = "unknown";
            if (p_sys->direct_rendering) {
                int sar_num = 1, sar_den = 1;
                if (p_dec->fmt_in.video.i_sar_num != 0 && p_dec->fmt_in.video.i_sar_den != 0) {
                    sar_num = p_dec->fmt_in.video.i_sar_num;
                    sar_den = p_dec->fmt_in.video.i_sar_den;
                }
                jni_SetAndroidSurfaceSizeEnv(env, width, height, width, height, sar_num, sar_den);
            } else {
                if (!GetVlcChromaFormat(p_sys->pixel_format,
                                        &p_dec->fmt_out.i_codec, &name)) {
                    msg_Err(p_dec, "color-format not recognized");
                    p_sys->error_state = true;
                    return;
                }
            }
            msg_Dbg(p_dec, "output: %d %s, %dx%d stride %d %d, crop %d %d %d %d",
                    p_sys->pixel_format, name, width, height, p_sys->stride, p_sys->slice_height,
                    p_sys->crop_left, p_sys->crop_top, crop_right, crop_bottom);
            p_dec->fmt_out.video.i_width = crop_right + 1 - p_sys->crop_left;
            p_dec->fmt_out.video.i_height = crop_bottom + 1 - p_sys->crop_top;
            if (p_sys->stride <= 0)
                p_sys->stride = width;
            if (p_sys->slice_height <= 0)
                p_sys->slice_height = height;
            if ((*env)->ExceptionOccurred(env))
                (*env)->ExceptionClear(env);
            ArchitectureSpecificCopyHooks(p_dec, p_sys->pixel_format, p_sys->slice_height,
                                          p_sys->stride, &p_sys->architecture_specific_data);
            if (p_sys->pixel_format == OMX_TI_COLOR_FormatYUV420PackedSemiPlanar) {
                p_sys->slice_height -= p_sys->crop_top/2;
                /* Reset crop top/left here, since the offset parameter already includes this.
                 * If we'd ignore the offset parameter in the BufferInfo, we could just keep
                 * the original slice height and apply the top/left cropping instead. */
                p_sys->crop_top = 0;
                p_sys->crop_left = 0;
            }
            if (IgnoreOmxDecoderPadding(p_sys->name)) {
                p_sys->slice_height = 0;
                p_sys->stride = p_dec->fmt_out.video.i_width;
            }
        } else {
            return;
        }
    }
}
