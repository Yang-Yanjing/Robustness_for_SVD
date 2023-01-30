/** Initialize the colour space conversion matrix */
static VdpStatus MixerSetupColors(filter_t *filter, const VdpProcamp *procamp,
                                  VdpCSCMatrix *restrict csc)
{
    filter_sys_t *sys = filter->p_sys;
    VdpStatus err;
    /* XXX: add some margin for padding... */
    VdpColorStandard std = (filter->fmt_in.video.i_height > 576 + 16)
                         ? VDP_COLOR_STANDARD_ITUR_BT_709
                         : VDP_COLOR_STANDARD_ITUR_BT_601;
    err = vdp_generate_csc_matrix(sys->vdp, procamp, std, csc);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s failure: %s", "color space matrix",
                vdp_get_error_string(sys->vdp, err));
        return err;
    }
    if (procamp != NULL)
    {
        sys->procamp.brightness = procamp->brightness;
        sys->procamp.contrast = procamp->contrast;
        sys->procamp.saturation = procamp->saturation;
        sys->procamp.hue = procamp->hue;
    }
    else
    {
        sys->procamp.brightness = 0.f;
        sys->procamp.contrast = 1.f;
        sys->procamp.saturation = 1.f;
        sys->procamp.hue = 0.f;
    }
    return VDP_STATUS_OK;
}
