}
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    int i,j;
    int i_lines = p_pic->p[ A_PLANE ].i_lines;
    int i_pitch = p_pic->p[ A_PLANE ].i_pitch;
    uint8_t *p_a = p_pic->p[ A_PLANE ].p_pixels;
    uint8_t *p_at;
    uint8_t *p_u = p_pic->p[ U_PLANE ].p_pixels;
    uint8_t *p_v = p_pic->p[ V_PLANE ].p_pixels;
    uint8_t umin, umax, vmin, vmax;
    if( p_pic->format.i_chroma != VLC_CODEC_YUVA )
    {
        msg_Err( p_filter,
                 "Unsupported input chroma \"%4.4s\". "
                 "Bluescreen can only use \"YUVA\".",
                 (char*)&p_pic->format.i_chroma );
        return NULL;
    }
    p_sys->p_at = xrealloc( p_sys->p_at,
                            i_lines * i_pitch * sizeof( uint8_t ) );
    p_at = p_sys->p_at;
    vlc_mutex_lock( &p_sys->lock );
    umin = p_sys->i_u - p_sys->i_ut >= 0x00 ? p_sys->i_u - p_sys->i_ut : 0x00;
    umax = p_sys->i_u + p_sys->i_ut <= 0xff ? p_sys->i_u + p_sys->i_ut : 0xff;
    vmin = p_sys->i_v - p_sys->i_vt >= 0x00 ? p_sys->i_v - p_sys->i_vt : 0x00;
    vmax = p_sys->i_v + p_sys->i_vt <= 0xff ? p_sys->i_v + p_sys->i_vt : 0xff;
    vlc_mutex_unlock( &p_sys->lock );
    for( i = 0; i < i_lines*i_pitch; i++ )
    {
        if(    p_u[i] < umax && p_u[i] > umin
            && p_v[i] < vmax && p_v[i] > vmin )
        {
            p_at[i] = 0x00;
        }
        else
        {
            p_at[i] = 0xff;
        }
    }
    /* Gaussian convolution to make it look cleaner */
    memset( p_a, 0, 2 * i_pitch );
    for( i = 2; i < i_lines - 2; i++ )
    {
        p_a[i*i_pitch] = 0x00;
        p_a[i*i_pitch+1] = 0x00;
        for( j = 2; j < i_pitch - 2; j ++ )
        {
            p_a[i*i_pitch+j] = (uint8_t)((
              /* 2 rows up */
                ( p_at[(i-2)*i_pitch+j-2]<<1 )
              + ( p_at[(i-2)*i_pitch+j-1]<<2 )
              + ( p_at[(i-2)*i_pitch+j]<<2 )
              + ( p_at[(i-2)*i_pitch+j+1]<<2 )
              + ( p_at[(i-2)*i_pitch+j+2]<<1 )
              /* 1 row up */
              + ( p_at[(i-1)*i_pitch+j-2]<<2 )
              + ( p_at[(i-1)*i_pitch+j-1]<<3 )
              + ( p_at[(i-1)*i_pitch+j]*12 )
              + ( p_at[(i-1)*i_pitch+j+1]<<3 )
              + ( p_at[(i-1)*i_pitch+j+2]<<2 )
              /* */
              + ( p_at[i*i_pitch+j-2]<<2 )
              + ( p_at[i*i_pitch+j-1]*12 )
              + ( p_at[i*i_pitch+j]<<4 )
              + ( p_at[i*i_pitch+j+1]*12 )
              + ( p_at[i*i_pitch+j+2]<<2 )
              /* 1 row down */
              + ( p_at[(i+1)*i_pitch+j-2]<<2 )
              + ( p_at[(i+1)*i_pitch+j-1]<<3 )
              + ( p_at[(i+1)*i_pitch+j]*12 )
              + ( p_at[(i+1)*i_pitch+j+1]<<3 )
              + ( p_at[(i+1)*i_pitch+j+2]<<2 )
              /* 2 rows down */
              + ( p_at[(i+2)*i_pitch+j-2]<<1 )
              + ( p_at[(i+2)*i_pitch+j-1]<<2 )
              + ( p_at[(i+2)*i_pitch+j]<<2 )
              + ( p_at[(i+2)*i_pitch+j+1]<<2 )
              + ( p_at[(i+2)*i_pitch+j+2]<<1 )
              )/152);
              if( p_a[i*i_pitch+j] < 0xbf ) p_a[i*i_pitch+j] = 0x00;
        }
    }
    return p_pic;
}
