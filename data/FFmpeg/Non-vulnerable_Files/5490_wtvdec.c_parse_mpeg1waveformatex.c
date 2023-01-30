static void parse_mpeg1waveformatex(AVStream *st)
{
    
    switch (AV_RL16(st->codec->extradata)) {
    case 0x0001 : st->codec->codec_id = AV_CODEC_ID_MP1; break;
    case 0x0002 : st->codec->codec_id = AV_CODEC_ID_MP2; break;
    case 0x0004 : st->codec->codec_id = AV_CODEC_ID_MP3; break;
    }
    st->codec->bit_rate = AV_RL32(st->codec->extradata + 2); 
    
    switch (AV_RL16(st->codec->extradata + 6)) {
    case 1 :
    case 2 :
    case 4 : st->codec->channels       = 2;
             st->codec->channel_layout = AV_CH_LAYOUT_STEREO;
             break;
    case 8 : st->codec->channels       = 1;
             st->codec->channel_layout = AV_CH_LAYOUT_MONO;
             break;
    }
}
