 *****************************************************************************/
static void DbInit( filter_sys_t * p_sys )
{
    float *pf_lin_data = p_sys->pf_lin_data;
    float *pf_db_data = p_sys->pf_db_data;
    /* Fill linear lookup table */
    for( int i = 0; i < LIN_TABLE_SIZE; i++ )
    {
        pf_lin_data[i] = powf( 10.0f, ( ( DB_MAX - DB_MIN ) *
                   (float)i / LIN_TABLE_SIZE + DB_MIN ) / 20.0f );
    }
    /* Fill logarithmic lookup table */
    for( int i = 0; i < DB_TABLE_SIZE; i++ )
    {
        pf_db_data[i] = 20.0f * log10f( ( LIN_MAX - LIN_MIN ) *
                   (float)i / DB_TABLE_SIZE + LIN_MIN );
    }
}
