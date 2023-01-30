#include "playlist_internal.h"
audio_output_t *playlist_GetAout(playlist_t *pl)
{
    /* NOTE: it is assumed that the input resource exists. In practice,
     * the playlist must have been activated. This is automatic when calling
     * pl_Get(). FIXME: input resources are deleted at deactivation, this can
     * be too early. */
    playlist_private_t *sys = pl_priv(pl);
    return input_resource_HoldAout(sys->p_input_resource);
}
