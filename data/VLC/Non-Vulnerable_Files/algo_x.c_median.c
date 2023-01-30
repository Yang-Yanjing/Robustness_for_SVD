}
static inline int median( int a, int b, int c )
{
    int min = a, max =a;
    if( b < min )
        min = b;
    else
        max = b;
    if( c < min )
        min = c;
    else if( c > max )
        max = c;
    return a + b + c - min - max;
}
