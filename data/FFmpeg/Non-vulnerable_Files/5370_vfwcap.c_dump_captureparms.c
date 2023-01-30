static void dump_captureparms(AVFormatContext *s, CAPTUREPARMS *cparms)
{
    av_log(s, AV_LOG_DEBUG, "CAPTUREPARMS\n");
    dstruct(s, cparms, dwRequestMicroSecPerFrame, "lu");
    dstruct(s, cparms, fMakeUserHitOKToCapture, "d");
    dstruct(s, cparms, wPercentDropForError, "u");
    dstruct(s, cparms, fYield, "d");
    dstruct(s, cparms, dwIndexSize, "lu");
    dstruct(s, cparms, wChunkGranularity, "u");
    dstruct(s, cparms, fUsingDOSMemory, "d");
    dstruct(s, cparms, wNumVideoRequested, "u");
    dstruct(s, cparms, fCaptureAudio, "d");
    dstruct(s, cparms, wNumAudioRequested, "u");
    dstruct(s, cparms, vKeyAbort, "u");
    dstruct(s, cparms, fAbortLeftMouse, "d");
    dstruct(s, cparms, fAbortRightMouse, "d");
    dstruct(s, cparms, fLimitEnabled, "d");
    dstruct(s, cparms, wTimeLimit, "u");
    dstruct(s, cparms, fMCIControl, "d");
    dstruct(s, cparms, fStepMCIDevice, "d");
    dstruct(s, cparms, dwMCIStartTime, "lu");
    dstruct(s, cparms, dwMCIStopTime, "lu");
    dstruct(s, cparms, fStepCaptureAt2x, "d");
    dstruct(s, cparms, wStepCaptureAverageFrames, "u");
    dstruct(s, cparms, dwAudioBufferSize, "lu");
    dstruct(s, cparms, fDisableWriteCache, "d");
    dstruct(s, cparms, AVStreamMaster, "u");
}
