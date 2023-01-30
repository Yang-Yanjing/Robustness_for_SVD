#include <libavutil/mem.h>
static inline void avcodec_free_context( AVCodecContext **ctx )
{
    if( !*ctx )
        return;
    av_free( (*ctx)->extradata );
    av_free( *ctx );
    *ctx = NULL;
}
