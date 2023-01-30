}
static void DrawZoomStatus( uint8_t *pb_dst, int i_pitch, int i_width, int i_height,
                            int i_offset_x, int i_offset_y, bool b_visible )
{
    static const char *p_hide =
        "X   X X      XXXX   XXXXX  XXX   XXX  XX XX   X   X XXXXX XXXX  XXXXXL"
        "X   X X     X          X  X   X X   X X X X   X   X   X   X   X X    L"
        " X X  X     X         X   X   X X   X X   X   XXXXX   X   X   X XXXX L"
        " X X  X     X        X    X   X X   X X   X   X   X   X   X   X X    L"
        "  X   XXXXX  XXXX   XXXXX  XXX   XXX  X   X   X   X XXXXX XXXX  XXXXXL";
    static const char *p_show = 
        "X   X X      XXXX   XXXXX  XXX   XXX  XX XX    XXXX X   X  XXX  X   XL"
        "X   X X     X          X  X   X X   X X X X   X     X   X X   X X   XL"
        " X X  X     X         X   X   X X   X X   X    XXX  XXXXX X   X X X XL"
        " X X  X     X        X    X   X X   X X   X       X X   X X   X X X XL"
        "  X   XXXXX  XXXX   XXXXX  XXX   XXX  X   X   XXXX  X   X  XXX   X X L";
    const char *p_draw = b_visible ? p_hide : p_show;
    int i, y, x;
    for( i = 0, x = i_offset_x, y = i_offset_y; p_draw[i] != '\0'; i++ )
    {
        if( p_draw[i] == 'X' )
        {
            if( x < i_width && y < i_height )
                pb_dst[y*i_pitch + x] = 0xff;
            x++;
        }
        else if( p_draw[i] == ' ' )
        {
            x++;
        }
        else if( p_draw[i] == 'L' )
        {
            x = i_offset_x;
            y++;
        }
    }
}
