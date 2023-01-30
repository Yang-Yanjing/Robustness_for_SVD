}
static void Direct3DSetupVertices(CUSTOMVERTEX *vertices,
                                  const RECT *src, const RECT *src_clipped,
                                  const RECT dst,
                                  int alpha,
                                  video_orientation_t orientation)
{
    /* Vertices of the dst rectangle in the unrotated (clockwise) order. */
    const int vertices_coords[4][2] = {
        { dst.left,  dst.top    },
        { dst.right, dst.top    },
        { dst.right, dst.bottom },
        { dst.left,  dst.bottom },
    };
    /* Compute index remapping necessary to implement the rotation. */
    int vertex_order[4];
    orientationVertexOrder(orientation, vertex_order);
    for (int i = 0; i < 4; ++i) {
        vertices[i].x  = vertices_coords[vertex_order[i]][0];
        vertices[i].y  = vertices_coords[vertex_order[i]][1];
    }
    float right = (float)src_clipped->right / (float)src->right;
    float left = (float)src_clipped->left / (float)src->right;
    float top = (float)src_clipped->top / (float)src->bottom;
    float bottom = (float)src_clipped->bottom / (float)src->bottom;
    vertices[0].tu = left;
    vertices[0].tv = top;
    vertices[1].tu = right;
    vertices[1].tv = top;
    vertices[2].tu = right;
    vertices[2].tv = bottom;
    vertices[3].tu = left;
    vertices[3].tv = bottom;
    for (int i = 0; i < 4; i++) {
        /* -0.5f is a "feature" of DirectX and it seems to apply to Direct3d also */
        /* http://www.sjbrown.co.uk/2003/05/01/fix-directx-rasterisation/ */
        vertices[i].x -= 0.5;
        vertices[i].y -= 0.5;
        vertices[i].z       = 0.0f;
        vertices[i].rhw     = 1.0f;
        vertices[i].diffuse = D3DCOLOR_ARGB(alpha, 255, 255, 255);
    }
}
}
static void Direct3DSetupVertices(CUSTOMVERTEX *vertices,
                                  const RECT *src, const RECT *src_clipped,
                                  const RECT dst,
                                  int alpha,
                                  video_orientation_t orientation)
{
    /* Vertices of the dst rectangle in the unrotated (clockwise) order. */
    const int vertices_coords[4][2] = {
        { dst.left,  dst.top    },
        { dst.right, dst.top    },
        { dst.right, dst.bottom },
        { dst.left,  dst.bottom },
    };
