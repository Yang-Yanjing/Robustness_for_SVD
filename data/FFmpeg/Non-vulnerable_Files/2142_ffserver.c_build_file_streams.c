static void build_file_streams(void)
{
    FFServerStream *stream;
    AVFormatContext *infile;
    int i, ret;
    
    for(stream = config.first_stream; stream; stream = stream->next) {
        infile = NULL;
        if (stream->stream_type != STREAM_TYPE_LIVE || stream->feed)
            continue;
        
        
        
        

        if (stream->fmt && !strcmp(stream->fmt->name, "rtp"))
            av_dict_set(&stream->in_opts, "mpeg2ts_compute_pcr", "1", 0);
        if (!stream->feed_filename[0]) {
            http_log("Unspecified feed file for stream '%s'\n",
                     stream->filename);
            goto fail;
        }
        http_log("Opening feed file '%s' for stream '%s'\n",
                 stream->feed_filename, stream->filename);
        ret = avformat_open_input(&infile, stream->feed_filename,
                                  stream->ifmt, &stream->in_opts);
        if (ret < 0) {
            http_log("Could not open '%s': %s\n", stream->feed_filename,
                     av_err2str(ret));
            
        fail:
            remove_stream(stream);
        } else {
            

            if (avformat_find_stream_info(infile, NULL) < 0) {
                http_log("Could not find codec parameters from '%s'\n",
                         stream->feed_filename);
                avformat_close_input(&infile);
                goto fail;
            }
            for(i=0;i<infile->nb_streams;i++)
                add_av_stream1(stream, infile->streams[i]->codec, 1);
            avformat_close_input(&infile);
        }
    }
}
