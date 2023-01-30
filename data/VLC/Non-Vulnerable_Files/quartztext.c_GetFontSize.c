}
static int GetFontSize(filter_t *p_filter)
{
    int i_size = 0;
    int i_ratio = var_CreateGetInteger( p_filter, "quartztext-rel-fontsize" );
    if( i_ratio > 0 )
        i_size = (int)p_filter->fmt_out.video.i_height / i_ratio;
    if( i_size <= 0 )
    {
        msg_Warn( p_filter, "invalid fontsize, using 12" );
        i_size = 12;
    }
    return i_size;
}
