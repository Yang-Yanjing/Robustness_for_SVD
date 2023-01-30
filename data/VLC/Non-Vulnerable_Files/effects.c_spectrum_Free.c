}
static void spectrum_Free( void *data )
{
    spectrum_data *p_data = data;
    if( p_data != NULL )
    {
        free( p_data->peaks );
        free( p_data->prev_heights );
        free( p_data->p_prev_s16_buff );
        free( p_data );
    }
}
