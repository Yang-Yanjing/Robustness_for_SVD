}
static float Db2Lin( float f_db, filter_sys_t * p_sys )
{
    float f_scale = ( f_db - DB_MIN ) * LIN_TABLE_SIZE / ( DB_MAX - DB_MIN );
    int i_base = Round( f_scale - 0.5f );
    float f_ofs = f_scale - i_base;
    float *pf_lin_data = p_sys->pf_lin_data;
    if( i_base < 1 )
    {
        return 0.0f;
    }
    else if( i_base > LIN_TABLE_SIZE - 3 )
    {
        return pf_lin_data[LIN_TABLE_SIZE - 2];
    }
#ifdef DB_DEFAULT_CUBE
    return CubeInterp( f_ofs, pf_lin_data[i_base - 1],
                              pf_lin_data[i_base],
                              pf_lin_data[i_base + 1],
                              pf_lin_data[i_base + 2] );
#else
    return ( 1.0f - f_ofs ) * pf_lin_data[i_base]
                  + f_ofs   * pf_lin_data[i_base + 1];
#endif
}
