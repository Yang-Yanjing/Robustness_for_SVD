/* Cubic interpolation function */
static float CubeInterp( const float f_fr, const float f_inm1,
                                           const float f_in,
                                           const float f_inp1,
                                           const float f_inp2 )
{
    return f_in + 0.5f * f_fr * ( f_inp1 - f_inm1 +
         f_fr * ( 4.0f * f_inp1 + 2.0f * f_inm1 - 5.0f * f_in - f_inp2 +
         f_fr * ( 3.0f * ( f_in - f_inp1 ) - f_inm1 + f_inp2 ) ) );
}
