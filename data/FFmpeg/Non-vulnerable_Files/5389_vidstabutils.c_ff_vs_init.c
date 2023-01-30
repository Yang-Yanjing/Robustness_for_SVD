void ff_vs_init(void)
{
    vs_malloc  = av_malloc;
    vs_zalloc  = av_mallocz;
    vs_realloc = av_realloc;
    vs_free    = av_free;
    VS_ERROR_TYPE = AV_LOG_ERROR;
    VS_WARN_TYPE  = AV_LOG_WARNING;
    VS_INFO_TYPE  = AV_LOG_INFO;
    VS_MSG_TYPE   = AV_LOG_VERBOSE;
    vs_log   = vs2av_log;
    VS_ERROR = 0;
    VS_OK    = 1;
}
