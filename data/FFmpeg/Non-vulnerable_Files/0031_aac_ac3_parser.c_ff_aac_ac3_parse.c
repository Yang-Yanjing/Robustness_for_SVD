int ff_aac_ac3_parse(AVCodecParserContext *s1,
                     AVCodecContext *avctx,
                     const uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size)
{
    AACAC3ParseContext *s = s1->priv_data;
    ParseContext *pc = &s->pc;
    int len, i;
    int new_frame_start;
get_next:
    i=END_NOT_FOUND;
    if(s->remaining_size <= buf_size){
        if(s->remaining_size && !s->need_next_header){
            i= s->remaining_size;
            s->remaining_size = 0;
        }else{ 
            len=0;
            for(i=s->remaining_size; i<buf_size; i++){
                s->state = (s->state<<8) + buf[i];
                if((len=s->sync(s->state, s, &s->need_next_header, &new_frame_start)))
                    break;
            }
            if(len<=0){
                i=END_NOT_FOUND;
            }else{
                s->state=0;
                i-= s->header_size -1;
                s->remaining_size = len;
                if(!new_frame_start || pc->index+i<=0){
                    s->remaining_size += i;
                    goto get_next;
                }
            }
        }
    }
    if(ff_combine_frame(pc, i, &buf, &buf_size)<0){
        s->remaining_size -= FFMIN(s->remaining_size, buf_size);
        *poutbuf = NULL;
        *poutbuf_size = 0;
        return buf_size;
    }
    *poutbuf = buf;
    *poutbuf_size = buf_size;
    
    if(s->codec_id)
        avctx->codec_id = s->codec_id;
    



    if (avctx->codec_id != AV_CODEC_ID_AAC) {
        avctx->sample_rate = s->sample_rate;
        
#if FF_API_REQUEST_CHANNELS
FF_DISABLE_DEPRECATION_WARNINGS
        if (avctx->request_channels == 1)
            avctx->request_channel_layout = AV_CH_LAYOUT_MONO;
        else if (avctx->request_channels == 2)
            avctx->request_channel_layout = AV_CH_LAYOUT_STEREO;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
        if (s->channels > 1 &&
            avctx->request_channel_layout == AV_CH_LAYOUT_MONO) {
            avctx->channels       = 1;
            avctx->channel_layout = AV_CH_LAYOUT_MONO;
        } else if (s->channels > 2 &&
                   avctx->request_channel_layout == AV_CH_LAYOUT_STEREO) {
            avctx->channels       = 2;
            avctx->channel_layout = AV_CH_LAYOUT_STEREO;
        } else {
            avctx->channels = s->channels;
            avctx->channel_layout = s->channel_layout;
        }
        s1->duration = s->samples;
        avctx->audio_service_type = s->service_type;
    }
    avctx->bit_rate = s->bit_rate;
    return i;
}
