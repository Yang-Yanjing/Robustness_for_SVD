/* A set of branchless clipping operations from Laurent de Soras */
static float Max( float f_x, float f_a )
{
    f_x -= f_a;
    f_x += fabs( f_x );
    f_x *= 0.5;
    f_x += f_a;
    return f_x;
}
