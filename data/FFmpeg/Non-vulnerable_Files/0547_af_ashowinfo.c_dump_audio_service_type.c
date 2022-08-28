static void dump_audio_service_type(AVFilterContext *ctx, AVFrameSideData *sd)
{
    enum AVAudioServiceType *ast;
    av_log(ctx, AV_LOG_INFO, "audio service type: ");
    if (sd->size < sizeof(*ast)) {
        av_log(ctx, AV_LOG_INFO, "invalid data");
        return;
    }
    ast = (enum AVAudioServiceType*)sd->data;
    switch (*ast) {
    case AV_AUDIO_SERVICE_TYPE_MAIN:              av_log(ctx, AV_LOG_INFO, "Main Audio Service"); break;
    case AV_AUDIO_SERVICE_TYPE_EFFECTS:           av_log(ctx, AV_LOG_INFO, "Effects");            break;
    case AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED: av_log(ctx, AV_LOG_INFO, "Visually Impaired");  break;
    case AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED:  av_log(ctx, AV_LOG_INFO, "Hearing Impaired");   break;
    case AV_AUDIO_SERVICE_TYPE_DIALOGUE:          av_log(ctx, AV_LOG_INFO, "Dialogue");           break;
    case AV_AUDIO_SERVICE_TYPE_COMMENTARY:        av_log(ctx, AV_LOG_INFO, "Commentary");         break;
    case AV_AUDIO_SERVICE_TYPE_EMERGENCY:         av_log(ctx, AV_LOG_INFO, "Emergency");          break;
    case AV_AUDIO_SERVICE_TYPE_VOICE_OVER:        av_log(ctx, AV_LOG_INFO, "Voice Over");         break;
    case AV_AUDIO_SERVICE_TYPE_KARAOKE:           av_log(ctx, AV_LOG_INFO, "Karaoke");            break;
    default:                                      av_log(ctx, AV_LOG_INFO, "unknown");            break;
    }
}
