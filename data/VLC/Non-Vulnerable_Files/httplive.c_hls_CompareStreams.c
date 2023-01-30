}
static int hls_CompareStreams( const void* a, const void* b )
{
    hls_stream_t*   stream_a = *(hls_stream_t**)a;
    hls_stream_t*   stream_b = *(hls_stream_t**)b;
    return stream_a->bandwidth - stream_b->bandwidth;
}
