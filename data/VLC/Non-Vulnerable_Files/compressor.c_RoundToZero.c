 * de Soras */
static void RoundToZero( float *pf_x )
{
    static const float f_anti_denormal = 1e-18;
    *pf_x += f_anti_denormal;
    *pf_x -= f_anti_denormal;
}
