static int opt_bitrate(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    if(!strcmp(opt, "ab")){
        av_dict_set(&o->g->codec_opts, "b:a", arg, 0);
        return 0;
    } else if(!strcmp(opt, "b")){
        av_log(NULL, AV_LOG_WARNING, "Please use -b:a or -b:v, -b is ambiguous\n");
        av_dict_set(&o->g->codec_opts, "b:v", arg, 0);
        return 0;
    }
    av_dict_set(&o->g->codec_opts, opt, arg, 0);
    return 0;
}
