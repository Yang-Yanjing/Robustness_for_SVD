int ffmpeg_parse_options(int argc, char **argv)
{
    OptionParseContext octx;
    uint8_t error[128];
    int ret;
    memset(&octx, 0, sizeof(octx));
    
    ret = split_commandline(&octx, argc, argv, options, groups,
                            FF_ARRAY_ELEMS(groups));
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error splitting the argument list: ");
        goto fail;
    }
    
    ret = parse_optgroup(NULL, &octx.global_opts);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error parsing global options: ");
        goto fail;
    }
    
    term_init();
    
    ret = open_files(&octx.groups[GROUP_INFILE], "input", open_input_file);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error opening input files: ");
        goto fail;
    }
    
    ret = init_complex_filters();
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error initializing complex filters.\n");
        goto fail;
    }
    
    ret = open_files(&octx.groups[GROUP_OUTFILE], "output", open_output_file);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "Error opening output files: ");
        goto fail;
    }
    check_filter_outputs();
fail:
    uninit_parse_context(&octx);
    if (ret < 0) {
        av_strerror(ret, error, sizeof(error));
        av_log(NULL, AV_LOG_FATAL, "%s\n", error);
    }
    return ret;
}
