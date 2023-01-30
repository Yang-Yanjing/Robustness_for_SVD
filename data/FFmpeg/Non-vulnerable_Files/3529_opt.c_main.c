int main(void)
{
    int i;
    av_log_set_level(AV_LOG_DEBUG);
    av_log_set_callback(log_callback_help);
    printf("Testing default values\n");
    {
        TestContext test_ctx = { 0 };
        test_ctx.class = &test_class;
        av_opt_set_defaults(&test_ctx);
        printf("num=%d\n", test_ctx.num);
        printf("toggle=%d\n", test_ctx.toggle);
        printf("string=%s\n", test_ctx.string);
        printf("escape=%s\n", test_ctx.escape);
        printf("flags=%d\n", test_ctx.flags);
        printf("rational=%d/%d\n", test_ctx.rational.num, test_ctx.rational.den);
        printf("video_rate=%d/%d\n", test_ctx.video_rate.num, test_ctx.video_rate.den);
        printf("width=%d height=%d\n", test_ctx.w, test_ctx.h);
        printf("pix_fmt=%s\n", av_get_pix_fmt_name(test_ctx.pix_fmt));
        printf("sample_fmt=%s\n", av_get_sample_fmt_name(test_ctx.sample_fmt));
        printf("duration=%"PRId64"\n", test_ctx.duration);
        printf("color=%d %d %d %d\n", test_ctx.color[0], test_ctx.color[1], test_ctx.color[2], test_ctx.color[3]);
        printf("channel_layout=%"PRId64"=%"PRId64"\n", test_ctx.channel_layout, (int64_t)AV_CH_LAYOUT_HEXAGONAL);
        if (test_ctx.binary)
            printf("binary=%x %x %x %x\n", ((uint8_t*)test_ctx.binary)[0], ((uint8_t*)test_ctx.binary)[1], ((uint8_t*)test_ctx.binary)[2], ((uint8_t*)test_ctx.binary)[3]);
        printf("binary_size=%d\n", test_ctx.binary_size);
        printf("num64=%"PRId64"\n", test_ctx.num64);
        printf("flt=%.6f\n", test_ctx.flt);
        printf("dbl=%.6f\n", test_ctx.dbl);
        av_opt_show2(&test_ctx, NULL, -1, 0);
        av_opt_free(&test_ctx);
    }
    printf("\nTesting av_opt_is_set_to_default()\n");
    {
        int ret;
        TestContext test_ctx = { 0 };
        const AVOption *o = NULL;
        test_ctx.class = &test_class;
        av_log_set_level(AV_LOG_QUIET);
        while (o = av_opt_next(&test_ctx, o)) {
            ret = av_opt_is_set_to_default_by_name(&test_ctx, o->name, 0);
            printf("name:%10s default:%d error:%s\n", o->name, !!ret, ret < 0 ? av_err2str(ret) : "");
        }
        av_opt_set_defaults(&test_ctx);
        while (o = av_opt_next(&test_ctx, o)) {
            ret = av_opt_is_set_to_default_by_name(&test_ctx, o->name, 0);
            printf("name:%10s default:%d error:%s\n", o->name, !!ret, ret < 0 ? av_err2str(ret) : "");
        }
        av_opt_free(&test_ctx);
    }
    printf("\nTest av_opt_serialize()\n");
    {
        TestContext test_ctx = { 0 };
        char *buf;
        test_ctx.class = &test_class;
        av_log_set_level(AV_LOG_QUIET);
        av_opt_set_defaults(&test_ctx);
        if (av_opt_serialize(&test_ctx, 0, 0, &buf, '=', ',') >= 0) {
            printf("%s\n", buf);
            av_opt_free(&test_ctx);
            memset(&test_ctx, 0, sizeof(test_ctx));
            test_ctx.class = &test_class;
            av_set_options_string(&test_ctx, buf, "=", ",");
            av_free(buf);
            if (av_opt_serialize(&test_ctx, 0, 0, &buf, '=', ',') >= 0) {
                printf("%s\n", buf);
                av_free(buf);
            }
        }
        av_opt_free(&test_ctx);
    }
    printf("\nTesting av_set_options_string()\n");
    {
        TestContext test_ctx = { 0 };
        static const char * const options[] = {
            "",
            ":",
            "=",
            "foo=:",
            ":=foo",
            "=foo",
            "foo=",
            "foo",
            "foo=val",
            "foo==val",
            "toggle=:",
            "string=:",
            "toggle=1 : foo",
            "toggle=100",
            "toggle==1",
            "flags=+mu-lame : num=42: toggle=0",
            "num=42 : string=blahblah",
            "rational=0 : rational=1/2 : rational=1/-1",
            "rational=-1/0",
            "size=1024x768",
            "size=pal",
            "size=bogus",
            "pix_fmt=yuv420p",
            "pix_fmt=2",
            "pix_fmt=bogus",
            "sample_fmt=s16",
            "sample_fmt=2",
            "sample_fmt=bogus",
            "video_rate=pal",
            "video_rate=25",
            "video_rate=30000/1001",
            "video_rate=30/1.001",
            "video_rate=bogus",
            "duration=bogus",
            "duration=123.45",
            "duration=1\\:23\\:45.67",
            "color=blue",
            "color=0x223300",
            "color=0x42FF07AA",
            "cl=stereo+downmix",
            "cl=foo",
            "bin=boguss",
            "bin=111",
            "bin=ffff",
            "num64=bogus",
            "num64=44",
            "num64=44.4",
            "num64=-1",
            "num64=101",
            "flt=bogus",
            "flt=2",
            "flt=2.2",
            "flt=-1",
            "flt=101",
            "dbl=bogus",
            "dbl=2",
            "dbl=2.2",
            "dbl=-1",
            "dbl=101",
        };
        test_ctx.class = &test_class;
        av_opt_set_defaults(&test_ctx);
        av_log_set_level(AV_LOG_QUIET);
        for (i=0; i < FF_ARRAY_ELEMS(options); i++) {
            int silence_log = !strcmp(options[i], "rational=-1/0"); 
            av_log(&test_ctx, AV_LOG_DEBUG, "Setting options string '%s'\n", options[i]);
            if (silence_log)
                av_log_set_callback(NULL);
            if (av_set_options_string(&test_ctx, options[i], "=", ":") < 0)
                printf("Error '%s'\n", options[i]);
            else
                printf("OK    '%s'\n", options[i]);
            av_log_set_callback(log_callback_help);
        }
        av_opt_free(&test_ctx);
    }
    printf("\nTesting av_opt_set_from_string()\n");
    {
        TestContext test_ctx = { 0 };
        static const char * const options[] = {
            "",
            "5",
            "5:hello",
            "5:hello:size=pal",
            "5:size=pal:hello",
            ":",
            "=",
            " 5 : hello : size = pal ",
            "a_very_long_option_name_that_will_need_to_be_ellipsized_around_here=42"
        };
        static const char * const shorthand[] = { "num", "string", NULL };
        test_ctx.class = &test_class;
        av_opt_set_defaults(&test_ctx);
        av_log_set_level(AV_LOG_QUIET);
        for (i=0; i < FF_ARRAY_ELEMS(options); i++) {
            av_log(&test_ctx, AV_LOG_DEBUG, "Setting options string '%s'\n", options[i]);
            if (av_opt_set_from_string(&test_ctx, options[i], shorthand, "=", ":") < 0)
                printf("Error '%s'\n", options[i]);
            else
                printf("OK    '%s'\n", options[i]);
        }
        av_opt_free(&test_ctx);
    }
    return 0;
}
