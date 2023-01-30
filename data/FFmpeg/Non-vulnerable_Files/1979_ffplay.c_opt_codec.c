static int opt_codec(void *optctx, const char *opt, const char *arg)
{
   const char *spec = strchr(opt, ':');
   if (!spec) {
       av_log(NULL, AV_LOG_ERROR,
              "No media specifier was specified in '%s' in option '%s'\n",
               arg, opt);
       return AVERROR(EINVAL);
   }
   spec++;
   switch (spec[0]) {
   case 'a' :    audio_codec_name = arg; break;
   case 's' : subtitle_codec_name = arg; break;
   case 'v' :    video_codec_name = arg; break;
   default:
       av_log(NULL, AV_LOG_ERROR,
              "Invalid media specifier '%s' in option '%s'\n", spec, opt);
       return AVERROR(EINVAL);
   }
   return 0;
}
