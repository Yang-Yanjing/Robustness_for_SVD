 */
static double GammaFactor( const panoramix_gamma_t *g, float f_value )
{
    if( f_value <= g->f_black_crush )
    {
        float f_input = f_value * g->f_black_level / g->f_black_crush + (1.0 - g->f_black_level);
        return pow( f_input, 1.0 / g->f_gamma );
    }
    else if( f_value >= g->f_white_crush )
    {
        float f_input = (f_value * (1.0 - (g->f_white_level + 1.0)) + (g->f_white_level + 1.0) * g->f_white_crush - 1.0) / (g->f_white_crush - 1.0);
        return pow( f_input, 1.0 / g->f_gamma );
    }
    return 1.0;
}
