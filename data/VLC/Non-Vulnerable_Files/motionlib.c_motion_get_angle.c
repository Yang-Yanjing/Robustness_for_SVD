 *****************************************************************************/
int motion_get_angle( motion_sensors_t *motion )
{
    const int filter_length = ARRAY_SIZE( motion->p_oldx );
    int i_x = GetOrientation( motion );
    motion->i_sum += i_x - motion->p_oldx[motion->i];
    motion->p_oldx[motion->i] = i_x;
    motion->i = ( motion->i + 1 ) % filter_length;
    i_x = motion->i_sum / filter_length;
    return i_x;
}
