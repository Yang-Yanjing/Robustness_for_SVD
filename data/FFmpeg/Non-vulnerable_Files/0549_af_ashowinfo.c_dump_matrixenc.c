static void dump_matrixenc(AVFilterContext *ctx, AVFrameSideData *sd)
{
    enum AVMatrixEncoding enc;
    av_log(ctx, AV_LOG_INFO, "matrix encoding: ");
    if (sd->size < sizeof(enum AVMatrixEncoding)) {
        av_log(ctx, AV_LOG_INFO, "invalid data");
        return;
    }
    enc = *(enum AVMatrixEncoding *)sd->data;
    switch (enc) {
    case AV_MATRIX_ENCODING_NONE:           av_log(ctx, AV_LOG_INFO, "none");                break;
    case AV_MATRIX_ENCODING_DOLBY:          av_log(ctx, AV_LOG_INFO, "Dolby Surround");      break;
    case AV_MATRIX_ENCODING_DPLII:          av_log(ctx, AV_LOG_INFO, "Dolby Pro Logic II");  break;
    case AV_MATRIX_ENCODING_DPLIIX:         av_log(ctx, AV_LOG_INFO, "Dolby Pro Logic IIx"); break;
    case AV_MATRIX_ENCODING_DPLIIZ:         av_log(ctx, AV_LOG_INFO, "Dolby Pro Logic IIz"); break;
    case AV_MATRIX_ENCODING_DOLBYEX:        av_log(ctx, AV_LOG_INFO, "Dolby EX");            break;
    case AV_MATRIX_ENCODING_DOLBYHEADPHONE: av_log(ctx, AV_LOG_INFO, "Dolby Headphone");     break;
    default:                                av_log(ctx, AV_LOG_WARNING, "unknown");          break;
    }
}
