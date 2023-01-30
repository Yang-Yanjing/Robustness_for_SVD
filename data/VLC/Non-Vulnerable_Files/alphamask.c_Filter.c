}
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_mutex_lock( &p_sys->mask_lock );
    plane_t *p_mask = p_sys->p_mask->p+A_PLANE;
    plane_t *p_apic = p_pic->p+A_PLANE;
    if(    p_mask->i_visible_pitch
        != p_apic->i_visible_pitch
        || p_mask->i_visible_lines
        != p_apic->i_visible_lines )
    {
        msg_Err( p_filter,
                  "Mask size (%d x %d) and image size (%d x %d) "
                  "don't match. The mask will not be applied.",
                  p_mask->i_visible_pitch,
                  p_mask->i_visible_lines,
                  p_apic->i_visible_pitch,
                  p_apic->i_visible_lines );
    }
    else
    {
        plane_CopyPixels( p_apic, p_mask );
    }
    vlc_mutex_unlock( &p_sys->mask_lock );
    return p_pic;
}
