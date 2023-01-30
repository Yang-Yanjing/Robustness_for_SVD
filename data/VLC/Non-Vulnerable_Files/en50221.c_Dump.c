#define T_DATA_MORE    0xA1
static void Dump( bool b_outgoing, uint8_t *p_data, int i_size )
{
#ifdef DEBUG_TPDU
    int i;
#define MAX_DUMP 256
    fprintf(stderr, "%s ", b_outgoing ? "-->" : "<--");
    for ( i = 0; i < i_size && i < MAX_DUMP; i++)
        fprintf(stderr, "%02X ", p_data[i]);
    fprintf(stderr, "%s\n", i_size >= MAX_DUMP ? "..." : "");
#else
    VLC_UNUSED(b_outgoing); VLC_UNUSED(p_data); VLC_UNUSED(i_size);
#endif
}
