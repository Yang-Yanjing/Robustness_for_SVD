}
static int Init( vlc_object_t *p_this, struct filter_sys_t * p_data
        , unsigned int i_nb_channels, uint32_t i_physical_channels
        , unsigned int i_rate )
{
    double d_x = var_InheritInteger( p_this, "headphone-dim" );
    double d_z = d_x;
    double d_z_rear = -d_x/3;
    double d_min = 0;
    unsigned int i_next_atomic_operation;
    int i_source_channel_offset;
    unsigned int i;
    if( var_InheritBool( p_this, "headphone-compensate" ) )
    {
        /* minimal distance to any speaker */
        if( i_physical_channels & AOUT_CHAN_REARCENTER )
        {
            d_min = d_z_rear;
        }
        else
        {
            d_min = d_z;
        }
    }
    /* Number of elementary operations */
    p_data->i_nb_atomic_operations = i_nb_channels * 2;
    if( i_physical_channels & AOUT_CHAN_CENTER )
    {
        p_data->i_nb_atomic_operations += 2;
    }
    p_data->p_atomic_operations = malloc( sizeof(struct atomic_operation_t)
            * p_data->i_nb_atomic_operations );
    if( p_data->p_atomic_operations == NULL )
        return -1;
    /* For each virtual speaker, computes elementary wave propagation time
     * to each ear */
    i_next_atomic_operation = 0;
    i_source_channel_offset = 0;
    if( i_physical_channels & AOUT_CHAN_LEFT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , -d_x , d_z , d_min , 2.0 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_RIGHT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , d_x , d_z , d_min , 2.0 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_MIDDLELEFT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , -d_x , 0 , d_min , 1.5 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_MIDDLERIGHT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , d_x , 0 , d_min , 1.5 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_REARLEFT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , -d_x , d_z_rear , d_min , 1.5 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_REARRIGHT )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , d_x , d_z_rear , d_min , 1.5 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_REARCENTER )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , 0 , -d_z , d_min , 1.5 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_CENTER )
    {
        /* having two center channels increases the spatialization effect */
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , d_x / 5.0 , d_z , d_min , 0.75 / i_nb_channels );
        i_next_atomic_operation += 2;
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , -d_x / 5.0 , d_z , d_min , 0.75 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    if( i_physical_channels & AOUT_CHAN_LFE )
    {
        ComputeChannelOperations( p_data , i_rate
                , i_next_atomic_operation , i_source_channel_offset
                , 0 , d_z_rear , d_min , 5.0 / i_nb_channels );
        i_next_atomic_operation += 2;
        i_source_channel_offset++;
    }
    /* Initialize the overflow buffer
     * we need it because the process induce a delay in the samples */
    p_data->i_overflow_buffer_size = 0;
    for( i = 0 ; i < p_data->i_nb_atomic_operations ; i++ )
    {
        if( p_data->i_overflow_buffer_size
                < p_data->p_atomic_operations[i].i_delay * 2 * sizeof (float) )
        {
            p_data->i_overflow_buffer_size
                = p_data->p_atomic_operations[i].i_delay * 2 * sizeof (float);
        }
    }
    p_data->p_overflow_buffer = (float *)malloc( p_data->i_overflow_buffer_size );
    if( p_data->p_overflow_buffer == NULL )
    {
        free( p_data->p_atomic_operations );
        return -1;
    }
    memset( p_data->p_overflow_buffer, 0 , p_data->i_overflow_buffer_size );
    return 0;
}
