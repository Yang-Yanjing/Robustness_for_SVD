}
static int Open (vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys;
    SLresult result;
    aout->sys = sys = calloc(1, sizeof(*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->p_so_handle = dlopen("libOpenSLES.so", RTLD_NOW);
    if (sys->p_so_handle == NULL)
    {
        msg_Err(aout, "Failed to load libOpenSLES");
        goto error;
    }
    sys->slCreateEnginePtr = dlsym(sys->p_so_handle, "slCreateEngine");
    if (unlikely(sys->slCreateEnginePtr == NULL))
    {
        msg_Err(aout, "Failed to load symbol slCreateEngine");
        goto error;
    }
#define OPENSL_DLSYM(dest, name)                       \
    do {                                                       \
        const SLInterfaceID *sym = dlsym(sys->p_so_handle, "SL_IID_"name);        \
        if (unlikely(sym == NULL))                             \
        {                                                      \
            msg_Err(aout, "Failed to load symbol SL_IID_"name); \
            goto error;                                        \
        }                                                      \
        sys->dest = *sym;                                           \
    } while(0)
    OPENSL_DLSYM(SL_IID_ANDROIDSIMPLEBUFFERQUEUE, "ANDROIDSIMPLEBUFFERQUEUE");
    OPENSL_DLSYM(SL_IID_ENGINE, "ENGINE");
    OPENSL_DLSYM(SL_IID_PLAY, "PLAY");
    OPENSL_DLSYM(SL_IID_VOLUME, "VOLUME");
#undef OPENSL_DLSYM
    // create engine
    result = sys->slCreateEnginePtr(&sys->engineObject, 0, NULL, 0, NULL, NULL);
    CHECK_OPENSL_ERROR("Failed to create engine");
    // realize the engine in synchronous mode
    result = Realize(sys->engineObject, SL_BOOLEAN_FALSE);
    CHECK_OPENSL_ERROR("Failed to realize engine");
    // get the engine interface, needed to create other objects
    result = GetInterface(sys->engineObject, sys->SL_IID_ENGINE, &sys->engineEngine);
    CHECK_OPENSL_ERROR("Failed to get the engine interface");
    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids1[] = { sys->SL_IID_VOLUME };
    const SLboolean req1[] = { SL_BOOLEAN_FALSE };
    result = CreateOutputMix(sys->engineEngine, &sys->outputMixObject, 1, ids1, req1);
    CHECK_OPENSL_ERROR("Failed to create output mix");
    // realize the output mix in synchronous mode
    result = Realize(sys->outputMixObject, SL_BOOLEAN_FALSE);
    CHECK_OPENSL_ERROR("Failed to realize output mix");
    vlc_mutex_init(&sys->lock);
    aout->start      = Start;
    aout->stop       = Stop;
    aout->time_get   = TimeGet;
    aout->play       = Play;
    aout->pause      = Pause;
    aout->flush      = Flush;
    aout->mute_set   = MuteSet;
    aout->volume_set = VolumeSet;
    return VLC_SUCCESS;
error:
    if (sys->outputMixObject)
        Destroy(sys->outputMixObject);
    if (sys->engineObject)
        Destroy(sys->engineObject);
    if (sys->p_so_handle)
        dlclose(sys->p_so_handle);
    free(sys);
    return VLC_EGENERIC;
}
