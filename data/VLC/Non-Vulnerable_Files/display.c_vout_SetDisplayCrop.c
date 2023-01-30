}
void vout_SetDisplayCrop(vout_display_t *vd,
                         unsigned crop_num, unsigned crop_den,
                         unsigned left, unsigned top, int right, int bottom)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (osys->crop.left  != (int)left  || osys->crop.top != (int)top ||
        osys->crop.right != right || osys->crop.bottom != bottom ||
        (crop_num > 0 && crop_den > 0 &&
         (crop_num != osys->crop.num || crop_den != osys->crop.den))) {
        osys->crop.left   = left;
        osys->crop.top    = top;
        osys->crop.right  = right;
        osys->crop.bottom = bottom;
        osys->crop.num    = crop_num;
        osys->crop.den    = crop_den;
        osys->ch_crop = true;
    }
}
