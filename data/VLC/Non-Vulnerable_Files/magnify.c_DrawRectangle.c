}
static void DrawRectangle( uint8_t *pb_dst, int i_pitch, int i_width, int i_height,
                           int x, int y, int i_w, int i_h )
{
    int dy;
    if( x + i_w > i_width || y + i_h > i_height )
        return;
    /* top line */
    memset( &pb_dst[y * i_pitch + x], 0xff, i_w );
    /* left and right */
    for( dy = 1; dy < i_h-1; dy++ )
    {
        pb_dst[(y+dy) * i_pitch + x +     0] = 0xff;
        pb_dst[(y+dy) * i_pitch + x + i_w-1] = 0xff;
    }
    /* bottom line */
    memset( &pb_dst[(y+i_h-1) * i_pitch + x], 0xff, i_w );
}
