}
static void Direct3DImportSubpicture(vout_display_t *vd,
                                     int *count_ptr, d3d_region_t **region,
                                     subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    int count = 0;
    for (subpicture_region_t *r = subpicture->p_region; r; r = r->p_next)
        count++;
    *count_ptr = count;
    *region    = calloc(count, sizeof(**region));
    if (*region == NULL) {
        *count_ptr = 0;
        return;
    }
    int i = 0;
    for (subpicture_region_t *r = subpicture->p_region; r; r = r->p_next, i++) {
        d3d_region_t *d3dr = &(*region)[i];
        HRESULT hr;
        d3dr->texture = NULL;
        for (int j = 0; j < sys->d3dregion_count; j++) {
            d3d_region_t *cache = &sys->d3dregion[j];
            if (cache->texture &&
                cache->format == sys->d3dregion_format &&
                cache->width  == r->fmt.i_width &&
                cache->height == r->fmt.i_height) {
#ifndef NDEBUG
                msg_Dbg(vd, "Reusing %dx%d texture for OSD",
                        cache->width, cache->height);
#endif
                *d3dr = *cache;
                memset(cache, 0, sizeof(*cache));
                break;
            }
        }
        if (!d3dr->texture) {
            d3dr->format = sys->d3dregion_format;
            d3dr->width  = r->fmt.i_width;
            d3dr->height = r->fmt.i_height;
            hr = IDirect3DDevice9_CreateTexture(sys->d3ddev,
                                                d3dr->width, d3dr->height,
                                                1,
                                                D3DUSAGE_DYNAMIC,
                                                d3dr->format,
                                                D3DPOOL_DEFAULT,
                                                &d3dr->texture,
                                                NULL);
            if (FAILED(hr)) {
                d3dr->texture = NULL;
                msg_Err(vd, "Failed to create %dx%d texture for OSD (hr=0x%0lX)",
                        d3dr->width, d3dr->height, hr);
                continue;
            }
#ifndef NDEBUG
            msg_Dbg(vd, "Created %dx%d texture for OSD",
                    r->fmt.i_width, r->fmt.i_height);
#endif
        }
        D3DLOCKED_RECT lock;
        hr = IDirect3DTexture9_LockRect(d3dr->texture, 0, &lock, NULL, 0);
        if (SUCCEEDED(hr)) {
            uint8_t  *dst_data   = lock.pBits;
            int       dst_pitch  = lock.Pitch;
            uint8_t  *src_data   = r->p_picture->p->p_pixels;
            int       src_pitch  = r->p_picture->p->i_pitch;
            for (unsigned y = 0; y < r->fmt.i_height; y++) {
                int copy_pitch = __MIN(dst_pitch, r->p_picture->p->i_pitch);
                if (d3dr->format == D3DFMT_A8B8G8R8) {
                    memcpy(&dst_data[y * dst_pitch], &src_data[y * src_pitch],
                           copy_pitch);
                } else {
                    for (int x = 0; x < copy_pitch; x += 4) {
                        dst_data[y * dst_pitch + x + 0] = src_data[y * src_pitch + x + 2];
                        dst_data[y * dst_pitch + x + 1] = src_data[y * src_pitch + x + 1];
                        dst_data[y * dst_pitch + x + 2] = src_data[y * src_pitch + x + 0];
                        dst_data[y * dst_pitch + x + 3] = src_data[y * src_pitch + x + 3];
                    }
                }
            }
            hr = IDirect3DTexture9_UnlockRect(d3dr->texture, 0);
            if (FAILED(hr))
                msg_Err(vd, "Failed to unlock the texture");
        } else {
            msg_Err(vd, "Failed to lock the texture");
        }
        /* Map the subpicture to sys->rect_dest */
        const RECT video = sys->rect_dest;
        const float scale_w = (float)(video.right  - video.left) / subpicture->i_original_picture_width;
        const float scale_h = (float)(video.bottom - video.top)  / subpicture->i_original_picture_height;
        RECT dst;
        dst.left   = video.left + scale_w * r->i_x,
        dst.right  = dst.left + scale_w * r->fmt.i_visible_width,
        dst.top    = video.top  + scale_h * r->i_y,
        dst.bottom = dst.top  + scale_h * r->fmt.i_visible_height;
        RECT src;
        src.left = 0;
        src.right = r->fmt.i_width;
        src.top = 0;
        src.bottom = r->fmt.i_height;
        RECT src_clipped;
        src_clipped.left = r->fmt.i_x_offset;
        src_clipped.right = r->fmt.i_x_offset + r->fmt.i_visible_width;
        src_clipped.top = r->fmt.i_y_offset;
        src_clipped.bottom = r->fmt.i_y_offset + r->fmt.i_visible_height;
        Direct3DSetupVertices(d3dr->vertex, &src, &src_clipped,
                              dst, subpicture->i_alpha * r->i_alpha / 255, ORIENT_NORMAL);
    }
}
