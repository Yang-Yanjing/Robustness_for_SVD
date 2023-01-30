};
static void gaussianblur_InitDistribution( filter_sys_t *p_sys )
{
    double f_sigma = p_sys->f_sigma;
    int i_dim = (int)(3.*f_sigma);
    type_t *pt_distribution = xmalloc( (2*i_dim+1) * sizeof( type_t ) );
    int x;
    for( x = -i_dim; x <= i_dim; x++ )
    {
        const float f_distribution = sqrt( exp(-(x*x)/(f_sigma*f_sigma) ) / (2.*M_PI*f_sigma*f_sigma) );
#ifdef DONT_USE_FLOATS
        const float f_factor = 1 << 8;
#else
        const float f_factor = 1;
#endif
        pt_distribution[i_dim+x] = (type_t)( f_distribution * f_factor );
        //printf("%f\n",(float)pt_distribution[i_dim+x]);
    }
    p_sys->i_dim = i_dim;
    p_sys->pt_distribution = pt_distribution;
}
