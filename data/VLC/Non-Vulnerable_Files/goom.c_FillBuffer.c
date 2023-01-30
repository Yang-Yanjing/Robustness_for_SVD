 *****************************************************************************/
static int FillBuffer( int16_t *p_data, int *pi_data,
                       date_t *pi_date, date_t *pi_date_end,
                       goom_thread_t *p_this )
{
    int i_samples = 0;
    block_t *p_block;
    while( *pi_data < 512 )
    {
        if( !p_this->i_blocks ) return VLC_EGENERIC;
        p_block = p_this->pp_blocks[0];
        i_samples = __MIN( (unsigned)(512 - *pi_data),
                p_block->i_buffer / sizeof(float) / p_this->i_channels );
        /* Date management */
        if( p_block->i_pts > VLC_TS_INVALID &&
            p_block->i_pts != date_Get( pi_date_end ) )
        {
           date_Set( pi_date_end, p_block->i_pts );
        }
        p_block->i_pts = VLC_TS_INVALID;
        date_Increment( pi_date_end, i_samples );
        while( i_samples > 0 )
        {
            float *p_float = (float *)p_block->p_buffer;
            p_data[*pi_data] = FloatToInt16( p_float[0] );
            if( p_this->i_channels > 1 )
                p_data[512 + *pi_data] = FloatToInt16( p_float[1] );
            (*pi_data)++;
            p_block->p_buffer += (sizeof(float) * p_this->i_channels);
            p_block->i_buffer -= (sizeof(float) * p_this->i_channels);
            i_samples--;
        }
        if( !p_block->i_buffer )
        {
            block_Release( p_block );
            p_this->i_blocks--;
            if( p_this->i_blocks )
                memmove( p_this->pp_blocks, p_this->pp_blocks + 1,
                         p_this->i_blocks * sizeof(block_t *) );
        }
    }
    *pi_date = *pi_date_end;
    *pi_data = 0;
    return VLC_SUCCESS;
}
