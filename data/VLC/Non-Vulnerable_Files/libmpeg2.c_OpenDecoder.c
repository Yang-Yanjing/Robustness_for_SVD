 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    uint32_t i_accel = 0;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_MPGV )
        return VLC_EGENERIC;
    /* Select onl recognized original format (standard mpeg video) */
    switch( p_dec->fmt_in.i_original_fourcc )
    {
    case VLC_FOURCC('m','p','g','1'):
    case VLC_FOURCC('m','p','g','2'):
    case VLC_FOURCC('m','p','g','v'):
    case VLC_FOURCC('P','I','M','1'):
    case VLC_FOURCC('h','d','v','2'):
        break;
    default:
        if( p_dec->fmt_in.i_original_fourcc )
            return VLC_EGENERIC;
        break;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = calloc( 1, sizeof(*p_sys)) ) == NULL )
        return VLC_ENOMEM;
    /* Initialize the thread properties */
    p_sys->p_mpeg2dec = NULL;
    p_sys->p_synchro  = NULL;
    p_sys->p_info     = NULL;
    p_sys->i_current_pts  = 0;
    p_sys->i_previous_pts = 0;
    p_sys->i_current_dts  = 0;
    p_sys->i_previous_dts = 0;
    p_sys->i_sar_num = 0;
    p_sys->i_sar_den = 0;
    p_sys->b_garbage_pic = false;
    p_sys->b_slice_i  = false;
    p_sys->b_second_field = false;
    p_sys->b_skip     = false;
    p_sys->b_preroll = false;
    DpbInit( p_dec );
    p_sys->i_cc_pts = 0;
    p_sys->i_cc_dts = 0;
    p_sys->i_cc_flags = 0;
#if MPEG2_RELEASE >= MPEG2_VERSION (0, 5, 0)
    p_dec->pf_get_cc = GetCc;
    cc_Init( &p_sys->cc );
#endif
    p_sys->p_gop_user_data = NULL;
    p_sys->i_gop_user_data = 0;
#if defined( __i386__ ) || defined( __x86_64__ )
    if( vlc_CPU_MMX() )
        i_accel |= MPEG2_ACCEL_X86_MMX;
    if( vlc_CPU_3dNOW() )
        i_accel |= MPEG2_ACCEL_X86_3DNOW;
    if( vlc_CPU_MMXEXT() )
        i_accel |= MPEG2_ACCEL_X86_MMXEXT;
#elif defined( __powerpc__ ) || defined( __ppc__ ) || defined( __ppc64__ )
    if( vlc_CPU_ALTIVEC() )
        i_accel |= MPEG2_ACCEL_PPC_ALTIVEC;
#elif defined(__arm__)
# ifdef MPEG2_ACCEL_ARM
    i_accel |= MPEG2_ACCEL_ARM;
# endif
# ifdef MPEG2_ACCEL_ARM_NEON
    if( vlc_CPU_ARM_NEON() )
        i_accel |= MPEG2_ACCEL_ARM_NEON;
# endif
    /* TODO: sparc */
#else
    /* If we do not know this CPU, trust libmpeg2's feature detection */
    i_accel = MPEG2_ACCEL_DETECT;
#endif
    /* Set CPU acceleration features */
    mpeg2_accel( i_accel );
    /* Initialize decoder */
    p_sys->p_mpeg2dec = mpeg2_init();
    if( p_sys->p_mpeg2dec == NULL)
    {
        msg_Err( p_dec, "mpeg2_init() failed" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->p_info = mpeg2_info( p_sys->p_mpeg2dec );
    p_dec->pf_decode_video = DecodeBlock;
    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = 0;
    return VLC_SUCCESS;
}
