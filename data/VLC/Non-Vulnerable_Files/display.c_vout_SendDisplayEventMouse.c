#include "vout_internal.h"
void vout_SendDisplayEventMouse(vout_thread_t *vout, const vlc_mouse_t *m)
{
    vlc_mouse_t tmp1, tmp2;
    /* The check on spu is needed as long as ALLOW_DUMMY_VOUT is defined */
    if (vout->p->spu && spu_ProcessMouse( vout->p->spu, m, &vout->p->display.vd->source))
        return;
    vlc_mutex_lock( &vout->p->filter.lock );
    if (vout->p->filter.chain_static && vout->p->filter.chain_interactive) {
        if (!filter_chain_MouseFilter(vout->p->filter.chain_interactive, &tmp1, m))
            m = &tmp1;
        if (!filter_chain_MouseFilter(vout->p->filter.chain_static,      &tmp2, m))
            m = &tmp2;
    }
    vlc_mutex_unlock( &vout->p->filter.lock );
    if (vlc_mouse_HasMoved(&vout->p->mouse, m)) {
        vout_SendEventMouseMoved(vout, m->i_x, m->i_y);
    }
    if (vlc_mouse_HasButton(&vout->p->mouse, m)) {
        for (unsigned button = 0; button < MOUSE_BUTTON_MAX; button++) {
            if (vlc_mouse_HasPressed(&vout->p->mouse, m, button))
                vout_SendEventMousePressed(vout, button);
            else if (vlc_mouse_HasReleased(&vout->p->mouse, m, button))
                vout_SendEventMouseReleased(vout, button);
        }
    }
    if (m->b_double_click)
        vout_SendEventMouseDoubleClick(vout);
    vout->p->mouse = *m;
}
