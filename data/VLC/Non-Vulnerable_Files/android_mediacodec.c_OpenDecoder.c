 *****************************************************************************/
static int OpenDecoder(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if (p_dec->fmt_in.i_cat != VIDEO_ES && !p_dec->b_force)
        return VLC_EGENERIC;
    const char *mime = NULL;
    switch (p_dec->fmt_in.i_codec) {
    case VLC_CODEC_H264: mime = "video/avc"; break;
    case VLC_CODEC_H263: mime = "video/3gpp"; break;
    case VLC_CODEC_MP4V: mime = "video/mp4v-es"; break;
    case VLC_CODEC_VC1:  mime = "video/wvc1"; break;
    case VLC_CODEC_VP8:  mime = "video/x-vnd.on2.vp8"; break;
    default:
        msg_Dbg(p_dec, "codec %4.4s not supported", (char *)&p_dec->fmt_in.i_codec);
        return VLC_EGENERIC;
    }
    size_t fmt_profile = 0;
    if (p_dec->fmt_in.i_codec == VLC_CODEC_H264)
        h264_get_profile_level(&p_dec->fmt_in, &fmt_profile, NULL, NULL);
    /* Allocate the memory needed to store the decoder's structure */
    if ((p_dec->p_sys = p_sys = calloc(1, sizeof(*p_sys))) == NULL)
        return VLC_ENOMEM;
    p_dec->pf_decode_video = DecodeVideo;
    p_dec->fmt_out.i_cat = p_dec->fmt_in.i_cat;
    p_dec->fmt_out.video = p_dec->fmt_in.video;
    p_dec->fmt_out.audio = p_dec->fmt_in.audio;
    p_dec->b_need_packetized = true;
    JNIEnv* env = NULL;
    jni_attach_thread(&env, THREAD_NAME);
    for (int i = 0; classes[i].name; i++) {
        *(jclass*)((uint8_t*)p_sys + classes[i].offset) =
            (*env)->FindClass(env, classes[i].name);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Warn(p_dec, "Unable to find class %s", classes[i].name);
            (*env)->ExceptionClear(env);
            goto error;
        }
    }
    jclass last_class;
    for (int i = 0; members[i].name; i++) {
        if (i == 0 || strcmp(members[i].class, members[i - 1].class))
            last_class = (*env)->FindClass(env, members[i].class);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Warn(p_dec, "Unable to find class %s", members[i].class);
            (*env)->ExceptionClear(env);
            goto error;
        }
        switch (members[i].type) {
        case METHOD:
            *(jmethodID*)((uint8_t*)p_sys + members[i].offset) =
                (*env)->GetMethodID(env, last_class, members[i].name, members[i].sig);
            break;
        case STATIC_METHOD:
            *(jmethodID*)((uint8_t*)p_sys + members[i].offset) =
                (*env)->GetStaticMethodID(env, last_class, members[i].name, members[i].sig);
            break;
        case FIELD:
            *(jfieldID*)((uint8_t*)p_sys + members[i].offset) =
                (*env)->GetFieldID(env, last_class, members[i].name, members[i].sig);
            break;
        }
        if ((*env)->ExceptionOccurred(env)) {
            msg_Warn(p_dec, "Unable to find the member %s in %s",
                     members[i].name, members[i].class);
            (*env)->ExceptionClear(env);
            goto error;
        }
    }
    int num_codecs = (*env)->CallStaticIntMethod(env, p_sys->media_codec_list_class,
                                                 p_sys->get_codec_count);
    jobject codec_name = NULL;
    for (int i = 0; i < num_codecs; i++) {
        jobject codec_capabilities = NULL;
        jobject profile_levels = NULL;
        jobject info = NULL;
        jobject name = NULL;
        jobject types = NULL;
        jsize name_len = 0;
        int profile_levels_len = 0, num_types = 0;
        const char *name_ptr = NULL;
        bool found = false;
        info = (*env)->CallStaticObjectMethod(env, p_sys->media_codec_list_class,
                                              p_sys->get_codec_info_at, i);
        if ((*env)->CallBooleanMethod(env, info, p_sys->is_encoder))
            goto loopclean;
        codec_capabilities = (*env)->CallObjectMethod(env, info, p_sys->get_capabilities_for_type,
                                                      (*env)->NewStringUTF(env, mime));
        if ((*env)->ExceptionOccurred(env)) {
            msg_Warn(p_dec, "Exception occurred in MediaCodecInfo.getCapabilitiesForType");
            (*env)->ExceptionClear(env);
            goto loopclean;
        } else if (codec_capabilities) {
            profile_levels = (*env)->GetObjectField(env, codec_capabilities, p_sys->profile_levels_field);
            if (profile_levels)
                profile_levels_len = (*env)->GetArrayLength(env, profile_levels);
        }
        msg_Dbg(p_dec, "Number of profile levels: %d", profile_levels_len);
        types = (*env)->CallObjectMethod(env, info, p_sys->get_supported_types);
        num_types = (*env)->GetArrayLength(env, types);
        name = (*env)->CallObjectMethod(env, info, p_sys->get_name);
        name_len = (*env)->GetStringUTFLength(env, name);
        name_ptr = (*env)->GetStringUTFChars(env, name, NULL);
        found = false;
        if (!strncmp(name_ptr, "OMX.google.", __MIN(11, name_len)))
            goto loopclean;
        for (int j = 0; j < num_types && !found; j++) {
            jobject type = (*env)->GetObjectArrayElement(env, types, j);
            if (!jstrcmp(env, type, mime)) {
                /* The mime type is matching for this component. We
                   now check if the capabilities of the codec is
                   matching the video format. */
                if (p_dec->fmt_in.i_codec == VLC_CODEC_H264 && fmt_profile) {
                    for (int i = 0; i < profile_levels_len && !found; ++i) {
                        jobject profile_level = (*env)->GetObjectArrayElement(env, profile_levels, i);
                        int omx_profile = (*env)->GetIntField(env, profile_level, p_sys->profile_field);
                        size_t codec_profile = convert_omx_to_profile_idc(omx_profile);
                        (*env)->DeleteLocalRef(env, profile_level);
                        if (codec_profile != fmt_profile)
                            continue;
                        /* Some encoders set the level too high, thus we ignore it for the moment.
                           We could try to guess the actual profile based on the resolution. */
                        found = true;
                    }
                }
                else
                    found = true;
            }
            (*env)->DeleteLocalRef(env, type);
        }
        if (found) {
            msg_Dbg(p_dec, "using %.*s", name_len, name_ptr);
            p_sys->name = malloc(name_len + 1);
            memcpy(p_sys->name, name_ptr, name_len);
            p_sys->name[name_len] = '\0';
            codec_name = name;
        }
loopclean:
        if (name)
            (*env)->ReleaseStringUTFChars(env, name, name_ptr);
        if (profile_levels)
            (*env)->DeleteLocalRef(env, profile_levels);
        if (types)
            (*env)->DeleteLocalRef(env, types);
        if (codec_capabilities)
            (*env)->DeleteLocalRef(env, codec_capabilities);
        if (info)
            (*env)->DeleteLocalRef(env, info);
        if (found)
            break;
    }
    if (!codec_name) {
        msg_Dbg(p_dec, "No suitable codec matching %s was found", mime);
        goto error;
    }
    // This method doesn't handle errors nicely, it crashes if the codec isn't found.
    // (The same goes for createDecoderByType.) This is fixed in latest AOSP and in 4.2,
    // but not in 4.1 devices.
    p_sys->codec = (*env)->CallStaticObjectMethod(env, p_sys->media_codec_class,
                                                  p_sys->create_by_codec_name, codec_name);
    if ((*env)->ExceptionOccurred(env)) {
        msg_Warn(p_dec, "Exception occurred in MediaCodec.createByCodecName.");
        (*env)->ExceptionClear(env);
        goto error;
    }
    p_sys->allocated = true;
    p_sys->codec = (*env)->NewGlobalRef(env, p_sys->codec);
    jobject format = (*env)->CallStaticObjectMethod(env, p_sys->media_format_class,
                         p_sys->create_video_format, (*env)->NewStringUTF(env, mime),
                         p_dec->fmt_in.video.i_width, p_dec->fmt_in.video.i_height);
    if (p_dec->fmt_in.i_extra) {
        // Allocate a byte buffer via allocateDirect in java instead of NewDirectByteBuffer,
        // since the latter doesn't allocate storage of its own, and we don't know how long
        // the codec uses the buffer.
        int buf_size = p_dec->fmt_in.i_extra + 20;
        jobject bytebuf = (*env)->CallStaticObjectMethod(env, p_sys->byte_buffer_class,
                                                         p_sys->allocate_direct, buf_size);
        uint32_t size = p_dec->fmt_in.i_extra;
        uint8_t *ptr = (*env)->GetDirectBufferAddress(env, bytebuf);
        if (p_dec->fmt_in.i_codec == VLC_CODEC_H264 && ((uint8_t*)p_dec->fmt_in.p_extra)[0] == 1) {
            convert_sps_pps(p_dec, p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra,
                            ptr, buf_size,
                            &size, &p_sys->nal_size);
        } else {
            memcpy(ptr, p_dec->fmt_in.p_extra, size);
        }
        (*env)->CallObjectMethod(env, bytebuf, p_sys->limit, size);
        (*env)->CallVoidMethod(env, format, p_sys->set_bytebuffer,
                               (*env)->NewStringUTF(env, "csd-0"), bytebuf);
        (*env)->DeleteLocalRef(env, bytebuf);
    }
    /* If the VideoPlayerActivity is not started, MediaCodec opaque
       direct rendering should be disabled since no surface will be
       attached to the JNI. */
    p_sys->direct_rendering = jni_IsVideoPlayerActivityCreated() && var_InheritBool(p_dec, CFG_PREFIX "dr");
    if (p_sys->direct_rendering) {
        jobject surf = jni_LockAndGetAndroidJavaSurface();
        if (surf) {
            // Configure MediaCodec with the Android surface.
            (*env)->CallVoidMethod(env, p_sys->codec, p_sys->configure, format, surf, NULL, 0);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Warn(p_dec, "Exception occurred in MediaCodec.configure with an output surface.");
                (*env)->ExceptionClear(env);
                jni_UnlockAndroidSurface();
                goto error;
            }
            p_dec->fmt_out.i_codec = VLC_CODEC_ANDROID_OPAQUE;
        } else {
            msg_Warn(p_dec, "Failed to get the Android Surface, disabling direct rendering.");
            p_sys->direct_rendering = false;
        }
        jni_UnlockAndroidSurface();
    }
    if (!p_sys->direct_rendering) {
        (*env)->CallVoidMethod(env, p_sys->codec, p_sys->configure, format, NULL, NULL, 0);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Warn(p_dec, "Exception occurred in MediaCodec.configure");
            (*env)->ExceptionClear(env);
            goto error;
        }
    }
    (*env)->CallVoidMethod(env, p_sys->codec, p_sys->start);
    if ((*env)->ExceptionOccurred(env)) {
        msg_Warn(p_dec, "Exception occurred in MediaCodec.start");
        (*env)->ExceptionClear(env);
        goto error;
    }
    p_sys->started = true;
    p_sys->input_buffers = (*env)->CallObjectMethod(env, p_sys->codec, p_sys->get_input_buffers);
    p_sys->output_buffers = (*env)->CallObjectMethod(env, p_sys->codec, p_sys->get_output_buffers);
    p_sys->buffer_info = (*env)->NewObject(env, p_sys->buffer_info_class, p_sys->buffer_info_ctor);
    p_sys->input_buffers = (*env)->NewGlobalRef(env, p_sys->input_buffers);
    p_sys->output_buffers = (*env)->NewGlobalRef(env, p_sys->output_buffers);
    p_sys->buffer_info = (*env)->NewGlobalRef(env, p_sys->buffer_info);
    p_sys->i_output_buffers = (*env)->GetArrayLength(env, p_sys->output_buffers);
    p_sys->inflight_picture = calloc(1, sizeof(picture_t*) * p_sys->i_output_buffers);
    if (!p_sys->inflight_picture)
        goto error;
    (*env)->DeleteLocalRef(env, format);
    jni_detach_thread();
    const int timestamp_fifo_size = 32;
    p_sys->timestamp_fifo = timestamp_FifoNew(timestamp_fifo_size);
    if (!p_sys->timestamp_fifo)
        goto error;
    return VLC_SUCCESS;
 error:
    jni_detach_thread();
    CloseDecoder(p_this);
    return VLC_EGENERIC;
}
