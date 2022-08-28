static int filter_frame(AVFilterLink *inlink, AVFrame *inpic)
{
    MCDeintContext *mcdeint = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *outpic, *frame_dec;
    AVPacket pkt = {0};
    int x, y, i, ret, got_frame = 0;
    outpic = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!outpic) {
        av_frame_free(&inpic);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outpic, inpic);
    inpic->quality = mcdeint->qp * FF_QP2LAMBDA;
    av_init_packet(&pkt);
    ret = avcodec_encode_video2(mcdeint->enc_ctx, &pkt, inpic, &got_frame);
    if (ret < 0)
        goto end;
    frame_dec = mcdeint->enc_ctx->coded_frame;
    for (i = 0; i < 3; i++) {
        int is_chroma = !!i;
        int w = FF_CEIL_RSHIFT(inlink->w, is_chroma);
        int h = FF_CEIL_RSHIFT(inlink->h, is_chroma);
        int fils = frame_dec->linesize[i];
        int srcs = inpic    ->linesize[i];
        int dsts = outpic   ->linesize[i];
        for (y = 0; y < h; y++) {
            if ((y ^ mcdeint->parity) & 1) {
                for (x = 0; x < w; x++) {
                    uint8_t *filp = &frame_dec->data[i][x + y*fils];
                    uint8_t *srcp = &inpic    ->data[i][x + y*srcs];
                    uint8_t *dstp = &outpic   ->data[i][x + y*dsts];
                    if (y > 0 && y < h-1){
                        int is_edge = x < 3 || x > w-4;
                        int diff0 = filp[-fils] - srcp[-srcs];
                        int diff1 = filp[+fils] - srcp[+srcs];
                        int temp = filp[0];
#define DELTA(j) av_clip(j, -x, w-1-x)
#define GET_SCORE_EDGE(j)\
   FFABS(srcp[-srcs+DELTA(-1+(j))] - srcp[+srcs+DELTA(-1-(j))])+\
   FFABS(srcp[-srcs+DELTA(j)     ] - srcp[+srcs+DELTA(  -(j))])+\
   FFABS(srcp[-srcs+DELTA(1+(j)) ] - srcp[+srcs+DELTA( 1-(j))])
#define GET_SCORE(j)\
   FFABS(srcp[-srcs-1+(j)] - srcp[+srcs-1-(j)])+\
   FFABS(srcp[-srcs  +(j)] - srcp[+srcs  -(j)])+\
   FFABS(srcp[-srcs+1+(j)] - srcp[+srcs+1-(j)])
#define CHECK_EDGE(j)\
    {   int score = GET_SCORE_EDGE(j);\
        if (score < spatial_score){\
            spatial_score = score;\
            diff0 = filp[-fils+DELTA(j)]    - srcp[-srcs+DELTA(j)];\
            diff1 = filp[+fils+DELTA(-(j))] - srcp[+srcs+DELTA(-(j))];\
#define CHECK(j)\
    {   int score = GET_SCORE(j);\
        if (score < spatial_score){\
            spatial_score= score;\
            diff0 = filp[-fils+(j)] - srcp[-srcs+(j)];\
            diff1 = filp[+fils-(j)] - srcp[+srcs-(j)];\
                        if (is_edge) {
                            int spatial_score = GET_SCORE_EDGE(0) - 1;
                            CHECK_EDGE(-1) CHECK_EDGE(-2) }} }}
                            CHECK_EDGE( 1) CHECK_EDGE( 2) }} }}
                        } else {
                            int spatial_score = GET_SCORE(0) - 1;
                            CHECK(-1) CHECK(-2) }} }}
                            CHECK( 1) CHECK( 2) }} }}
                        }
                        if (diff0 + diff1 > 0)
                            temp -= (diff0 + diff1 - FFABS(FFABS(diff0) - FFABS(diff1)) / 2) / 2;
                        else
                            temp -= (diff0 + diff1 + FFABS(FFABS(diff0) - FFABS(diff1)) / 2) / 2;
                        *filp = *dstp = temp > 255U ? ~(temp>>31) : temp;
                    } else {
                        *dstp = *filp;
                    }
                }
            }
        }
        for (y = 0; y < h; y++) {
            if (!((y ^ mcdeint->parity) & 1)) {
                for (x = 0; x < w; x++) {
                    frame_dec->data[i][x + y*fils] =
                    outpic   ->data[i][x + y*dsts] = inpic->data[i][x + y*srcs];
                }
            }
        }
    }
    mcdeint->parity ^= 1;
end:
    av_free_packet(&pkt);
    av_frame_free(&inpic);
    if (ret < 0) {
        av_frame_free(&outpic);
        return ret;
    }
    return ff_filter_frame(outlink, outpic);
}
