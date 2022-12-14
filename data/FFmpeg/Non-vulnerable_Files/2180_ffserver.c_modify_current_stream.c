static int modify_current_stream(HTTPContext *c, char *rates)
{
    int i;
    FFServerStream *req = c->stream;
    int action_required = 0;
    
    if (!req->feed)
        return 0;
    for (i = 0; i < req->nb_streams; i++) {
        AVCodecParameters *codec = req->streams[i]->codecpar;
        switch(rates[i]) {
            case 0:
                c->switch_feed_streams[i] = req->feed_streams[i];
                break;
            case 1:
                c->switch_feed_streams[i] = find_stream_in_feed(req->feed, codec, codec->bit_rate / 2);
                break;
            case 2:
                
                c->switch_feed_streams[i] = find_stream_in_feed(req->feed, codec, codec->bit_rate / 4);
#ifdef WANTS_OFF
                
                c->switch_feed_streams[i] = -2;
                c->feed_streams[i] = -2;
#endif
                break;
        }
        if (c->switch_feed_streams[i] >= 0 &&
            c->switch_feed_streams[i] != c->feed_streams[i]) {
            action_required = 1;
        }
    }
    return action_required;
}
