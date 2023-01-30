/* fpclassify() is C99, cannot be compiled into a C++90 file (on some systems) */
float undenormalise( float f )
{
    if( fpclassify( f ) == FP_SUBNORMAL  )
        return 0.0;
    return f;
}
