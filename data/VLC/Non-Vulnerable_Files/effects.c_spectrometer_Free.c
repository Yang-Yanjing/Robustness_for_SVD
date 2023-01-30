}
static void spectrometer_Free( void *data )
{
    spectrometer_data *p_data = data;
    if( p_data != NULL )
    {
        free( p_data->peaks );
        free( p_data->p_prev_s16_buff );
        free( p_data );
    }
}
