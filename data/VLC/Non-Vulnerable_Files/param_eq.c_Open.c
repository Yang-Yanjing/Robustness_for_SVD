 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t *)p_this;
    unsigned     i_samplerate;
    /* Allocate structure */
    filter_sys_t *p_sys = p_filter->p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_EGENERIC;
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    p_sys->f_lowf = var_InheritFloat( p_this, "param-eq-lowf");
    p_sys->f_lowgain = var_InheritFloat( p_this, "param-eq-lowgain");
    p_sys->f_highf = var_InheritFloat( p_this, "param-eq-highf");
    p_sys->f_highgain = var_InheritFloat( p_this, "param-eq-highgain");
    p_sys->f_f1 = var_InheritFloat( p_this, "param-eq-f1");
    p_sys->f_Q1 = var_InheritFloat( p_this, "param-eq-q1");
    p_sys->f_gain1 = var_InheritFloat( p_this, "param-eq-gain1");
    p_sys->f_f2 = var_InheritFloat( p_this, "param-eq-f2");
    p_sys->f_Q2 = var_InheritFloat( p_this, "param-eq-q2");
    p_sys->f_gain2 = var_InheritFloat( p_this, "param-eq-gain2");
    p_sys->f_f3 = var_InheritFloat( p_this, "param-eq-f3");
    p_sys->f_Q3 = var_InheritFloat( p_this, "param-eq-q3");
    p_sys->f_gain3 = var_InheritFloat( p_this, "param-eq-gain3");
    i_samplerate = p_filter->fmt_in.audio.i_rate;
    CalcPeakEQCoeffs(p_sys->f_f1, p_sys->f_Q1, p_sys->f_gain1,
                     i_samplerate, p_sys->coeffs+0*5);
    CalcPeakEQCoeffs(p_sys->f_f2, p_sys->f_Q2, p_sys->f_gain2,
                     i_samplerate, p_sys->coeffs+1*5);
    CalcPeakEQCoeffs(p_sys->f_f3, p_sys->f_Q3, p_sys->f_gain3,
                     i_samplerate, p_sys->coeffs+2*5);
    CalcShelfEQCoeffs(p_sys->f_lowf, 1, p_sys->f_lowgain, 0,
                      i_samplerate, p_sys->coeffs+3*5);
    CalcShelfEQCoeffs(p_sys->f_highf, 1, p_sys->f_highgain, 0,
                      i_samplerate, p_sys->coeffs+4*5);
    p_sys->p_state = (float*)calloc( p_filter->fmt_in.audio.i_channels*5*4,
                                     sizeof(float) );
    return VLC_SUCCESS;
}
