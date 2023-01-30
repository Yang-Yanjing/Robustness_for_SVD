int main(int argc, char **argv)
{
    OutputStream video_st = { 0 }, audio_st = { 0 };
    const char *filename;
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
    
    av_register_all();
    if (argc < 2) {
        printf("usage: %s output_file\n"
               "API example program to output a media file with libavformat.\n"
               "This program generates a synthetic audio and video stream, encodes and\n"
               "muxes them into a file named output_file.\n"
               "The output format is automatically guessed according to the file extension.\n"
               "Raw images can also be output by using '%%d' in the filename.\n"
               "\n", argv[0]);
        return 1;
    }
    filename = argv[1];
    if (argc > 3 && !strcmp(argv[2], "-flags")) {
        av_dict_set(&opt, argv[2]+1, argv[3], 0);
    }
    
    avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc)
        return 1;
    fmt = oc->oformat;
    

    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&video_st, oc, &video_codec, fmt->video_codec);
        have_video = 1;
        encode_video = 1;
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
        have_audio = 1;
        encode_audio = 1;
    }
    

    if (have_video)
        open_video(oc, video_codec, &video_st, opt);
    if (have_audio)
        open_audio(oc, audio_codec, &audio_st, opt);
    av_dump_format(oc, 0, filename, 1);
    
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", filename,
                    av_err2str(ret));
            return 1;
        }
    }
    
    ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
                av_err2str(ret));
        return 1;
    }
    while (encode_video || encode_audio) {
        
        if (encode_video &&
            (!encode_audio || av_compare_ts(video_st.next_pts, video_st.st->codec->time_base,
                                            audio_st.next_pts, audio_st.st->codec->time_base) <= 0)) {
            encode_video = !write_video_frame(oc, &video_st);
        } else {
            encode_audio = !write_audio_frame(oc, &audio_st);
        }
    }
    



    av_write_trailer(oc);
    
    if (have_video)
        close_stream(oc, &video_st);
    if (have_audio)
        close_stream(oc, &audio_st);
    if (!(fmt->flags & AVFMT_NOFILE))
        
        avio_closep(&oc->pb);
    
    avformat_free_context(oc);
    return 0;
}
