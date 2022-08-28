static void build_elision_headers(AVFormatContext *s)
{
    NUTContext *nut = s->priv_data;
    int i;
    
    
    static const uint8_t headers[][5] = {
        { 3, 0x00, 0x00, 0x01 },
        { 4, 0x00, 0x00, 0x01, 0xB6},
        { 2, 0xFF, 0xFA }, 
        { 2, 0xFF, 0xFB }, 
        { 2, 0xFF, 0xFC }, 
        { 2, 0xFF, 0xFD }, 
    };
    nut->header_count = 7;
    for (i = 1; i < nut->header_count; i++) {
        nut->header_len[i] = headers[i - 1][0];
        nut->header[i]     = &headers[i - 1][1];
    }
}
