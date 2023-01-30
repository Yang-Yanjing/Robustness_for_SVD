}
static float Lin2Db( float f_lin, filter_sys_t * p_sys )
{
    float f_scale = ( f_lin - LIN_MIN ) * DB_TABLE_SIZE / ( LIN_MAX - LIN_MIN );
    int i_base = Round( f_scale - 0.5f );
    float f_ofs = f_scale - i_base;
    float *pf_db_data = p_sys->pf_db_data;
    if( i_base < 2 )
    {
        return pf_db_data[2] * f_scale * 0.5f - 23.0f * ( 2.0f - f_scale );
    }
    else if( i_base > DB_TABLE_SIZE - 3 )
    {
        return pf_db_data[DB_TABLE_SIZE - 2];
    }
#ifdef DB_DEFAULT_CUBE
    return CubeInterp( f_ofs, pf_db_data[i_base - 1],
                              pf_db_data[i_base],
                              pf_db_data[i_base + 1],
                              pf_db_data[i_base + 2] );
#else
    return ( 1.0f - f_ofs ) * pf_db_data[i_base]
                  + f_ofs   * pf_db_data[i_base + 1];
#endif
}
