#include "aout_internal.h"
static filter_t *CreateFilter (vlc_object_t *obj, const char *type,
                               const char *name, filter_owner_sys_t *owner,
                               const audio_sample_format_t *infmt,
                               const audio_sample_format_t *outfmt)
{
    filter_t *filter = vlc_custom_create (obj, sizeof (*filter), type);
    if (unlikely(filter == NULL))
        return NULL;
    filter->p_owner = owner;
    filter->fmt_in.audio = *infmt;
    filter->fmt_in.i_codec = infmt->i_format;
    filter->fmt_out.audio = *outfmt;
    filter->fmt_out.i_codec = outfmt->i_format;
    filter->p_module = module_need (filter, type, name, false);
    if (filter->p_module == NULL)
    {
        /* If probing failed, formats shall not have been modified. */
        assert (AOUT_FMTS_IDENTICAL(&filter->fmt_in.audio, infmt));
        assert (AOUT_FMTS_IDENTICAL(&filter->fmt_out.audio, outfmt));
        vlc_object_release (filter);
        filter = NULL;
    }
    else
        assert (filter->pf_audio_filter != NULL);
    return filter;
}
