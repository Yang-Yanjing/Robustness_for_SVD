int main(int argc, char *argv[])
{
    struct AVMD5 *md5;
    AVFilterGraph *graph;
    AVFilterContext *src, *sink;
    AVFrame *frame;
    uint8_t errstr[1024];
    float duration;
    int err, nb_frames, i;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <duration>\n", argv[0]);
        return 1;
    }
    duration  = atof(argv[1]);
    nb_frames = duration * INPUT_SAMPLERATE / FRAME_SIZE;
    if (nb_frames <= 0) {
        fprintf(stderr, "Invalid duration: %s\n", argv[1]);
        return 1;
    }
    avfilter_register_all();
    
    frame  = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating the frame\n");
        return 1;
    }
    md5 = av_md5_alloc();
    if (!md5) {
        fprintf(stderr, "Error allocating the MD5 context\n");
        return 1;
    }
    
    err = init_filter_graph(&graph, &src, &sink);
    if (err < 0) {
        fprintf(stderr, "Unable to init filter graph:");
        goto fail;
    }
    
    for (i = 0; i < nb_frames; i++) {
        
        err = get_input(frame, i);
        if (err < 0) {
            fprintf(stderr, "Error generating input frame:");
            goto fail;
        }
        
        err = av_buffersrc_add_frame(src, frame);
        if (err < 0) {
            av_frame_unref(frame);
            fprintf(stderr, "Error submitting the frame to the filtergraph:");
            goto fail;
        }
        
        while ((err = av_buffersink_get_frame(sink, frame)) >= 0) {
            
            err = process_output(md5, frame);
            if (err < 0) {
                fprintf(stderr, "Error processing the filtered frame:");
                goto fail;
            }
            av_frame_unref(frame);
        }
        if (err == AVERROR(EAGAIN)) {
            
            continue;
        } else if (err == AVERROR_EOF) {
            
            break;
        } else if (err < 0) {
            
            fprintf(stderr, "Error filtering the data:");
            goto fail;
        }
    }
    avfilter_graph_free(&graph);
    av_frame_free(&frame);
    av_freep(&md5);
    return 0;
fail:
    av_strerror(err, errstr, sizeof(errstr));
    fprintf(stderr, "%s\n", errstr);
    return 1;
}
