vlc_module_end ()
static void *InitLibrary(struct aout_sys_t *p_sys)
{
    /* DL Open libmedia */
    void *p_library;
    p_library = dlopen("libmedia.so", RTLD_NOW|RTLD_LOCAL);
    if (!p_library)
        return NULL;
    /* Register symbols */
    p_sys->as_getOutputFrameCount = (AudioSystem_getOutputFrameCount)(dlsym(p_library, "_ZN7android11AudioSystem19getOutputFrameCountEPii"));
    p_sys->as_getOutputLatency = (AudioSystem_getOutputLatency)(dlsym(p_library, "_ZN7android11AudioSystem16getOutputLatencyEPji"));
    if(p_sys->as_getOutputLatency == NULL) {
        /* 4.1 Jellybean prototype */
        p_sys->as_getOutputLatency = (AudioSystem_getOutputLatency)(dlsym(p_library, "_ZN7android11AudioSystem16getOutputLatencyEPj19audio_stream_type_t"));
    }
    p_sys->as_getOutputSamplingRate = (AudioSystem_getOutputSamplingRate)(dlsym(p_library, "_ZN7android11AudioSystem21getOutputSamplingRateEPii"));
    p_sys->at_getMinFrameCount = (AudioTrack_getMinFrameCount)(dlsym(p_library, "_ZN7android10AudioTrack16getMinFrameCountEPiij"));
    if(p_sys->at_getMinFrameCount == NULL) {
        /* 4.1 Jellybean prototype */
        p_sys->at_getMinFrameCount = (AudioTrack_getMinFrameCount)(dlsym(p_library, "_ZN7android10AudioTrack16getMinFrameCountEPi19audio_stream_type_tj"));
    }
    p_sys->at_ctor = (AudioTrack_ctor)(dlsym(p_library, "_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_ii"));
    p_sys->at_ctor_legacy = (AudioTrack_ctor_legacy)(dlsym(p_library, "_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_i"));
    p_sys->at_dtor = (AudioTrack_dtor)(dlsym(p_library, "_ZN7android10AudioTrackD1Ev"));
    p_sys->at_initCheck = (AudioTrack_initCheck)(dlsym(p_library, "_ZNK7android10AudioTrack9initCheckEv"));
    p_sys->at_start = (AudioTrack_start)(dlsym(p_library, "_ZN7android10AudioTrack5startEv"));
    p_sys->at_stop = (AudioTrack_stop)(dlsym(p_library, "_ZN7android10AudioTrack4stopEv"));
    p_sys->at_write = (AudioTrack_write)(dlsym(p_library, "_ZN7android10AudioTrack5writeEPKvj"));
    p_sys->at_flush = (AudioTrack_flush)(dlsym(p_library, "_ZN7android10AudioTrack5flushEv"));
    p_sys->at_pause = (AudioTrack_pause)(dlsym(p_library, "_ZN7android10AudioTrack5pauseEv"));
    /* this symbol can have different names depending on the mangling */
    p_sys->at_getRenderPosition = (AudioTrack_getRenderPosition)(dlsym(p_library, "_ZN7android11AudioSystem17getRenderPositionEPjS1_i"));
    if (!p_sys->at_getRenderPosition)
        p_sys->at_getRenderPosition = (AudioTrack_getRenderPosition)(dlsym(p_library, "_ZN7android11AudioSystem17getRenderPositionEPjS1_19audio_stream_type_t"));
    /* We need the first 3 or the last 1 */
    if (!((p_sys->as_getOutputFrameCount && p_sys->as_getOutputLatency && p_sys->as_getOutputSamplingRate)
        || p_sys->at_getMinFrameCount)) {
        dlclose(p_library);
        return NULL;
    }
    // We need all the other Symbols
    if (!((p_sys->at_ctor || p_sys->at_ctor_legacy) && p_sys->at_dtor && p_sys->at_initCheck &&
           p_sys->at_start && p_sys->at_stop && p_sys->at_write && p_sys->at_flush)) {
        dlclose(p_library);
        return NULL;
    }
    return p_library;
}
