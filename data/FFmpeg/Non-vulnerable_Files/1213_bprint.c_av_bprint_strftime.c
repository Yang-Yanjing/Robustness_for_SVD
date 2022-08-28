void av_bprint_strftime(AVBPrint *buf, const char *fmt, const struct tm *tm)
{
    unsigned room;
    size_t l;
    if (!*fmt)
        return;
    while (1) {
        room = av_bprint_room(buf);
        if (room && (l = strftime(buf->str + buf->len, room, fmt, tm)))
            break;
        

        room = !room ? strlen(fmt) + 1 :
               room <= INT_MAX / 2 ? room * 2 : INT_MAX;
        if (av_bprint_alloc(buf, room)) {
            
            room = av_bprint_room(buf);
            if (room < 1024) {
                


                char buf2[1024];
                if ((l = strftime(buf2, sizeof(buf2), fmt, tm))) {
                    av_bprintf(buf, "%s", buf2);
                    return;
                }
            }
            if (room) {
                

                static const char txt[] = "[truncated strftime output]";
                memset(buf->str + buf->len, '!', room);
                memcpy(buf->str + buf->len, txt, FFMIN(sizeof(txt) - 1, room));
                av_bprint_grow(buf, room); 
            }
            return;
        }
    }
    av_bprint_grow(buf, l);
}
