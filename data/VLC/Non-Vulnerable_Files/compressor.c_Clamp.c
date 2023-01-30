}
static float Clamp( float f_x, float f_a, float f_b )
{
    const float f_x1 = fabs( f_x - f_a );
    const float f_x2 = fabs( f_x - f_b );
    f_x = f_x1 + f_a + f_b;
    f_x -= f_x2;
    f_x *= 0.5;
    return f_x;
}
