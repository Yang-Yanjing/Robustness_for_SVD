/* Round float to int using IEEE int* hack */
static int Round( float f_x )
{
    ls_pcast32 p;
    p.f = f_x;
    p.f += ( 3 << 22 );
    return p.i - 0x4b400000;
}
