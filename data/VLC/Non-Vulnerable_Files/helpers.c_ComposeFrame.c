/* See header for function doc. */
void ComposeFrame( filter_t *p_filter,
                   picture_t *p_outpic,
                   picture_t *p_inpic_top, picture_t *p_inpic_bottom,
                   compose_chroma_t i_output_chroma, bool swapped_uv_conversion )
{
    assert( p_outpic != NULL );
    assert( p_inpic_top != NULL );
    assert( p_inpic_bottom != NULL );
    /* Valid 4:2:0 chroma handling modes. */
    assert( i_output_chroma == CC_ALTLINE       ||
            i_output_chroma == CC_UPCONVERT     ||
            i_output_chroma == CC_SOURCE_TOP    ||
            i_output_chroma == CC_SOURCE_BOTTOM ||
            i_output_chroma == CC_MERGE );
    const bool b_upconvert_chroma = i_output_chroma == CC_UPCONVERT;
    for( int i_plane = 0 ; i_plane < p_inpic_top->i_planes ; i_plane++ )
    {
        bool b_is_chroma_plane = ( i_plane == U_PLANE || i_plane == V_PLANE );
        int i_out_plane;
        if( b_is_chroma_plane  &&  b_upconvert_chroma  && swapped_uv_conversion )
        {
            if( i_plane == U_PLANE )
                i_out_plane = V_PLANE;
            else /* V_PLANE */
                i_out_plane = U_PLANE;
        }
        else
        {
            i_out_plane = i_plane;
        }
        /* Copy luma or chroma, alternating between input fields. */
        if( !b_is_chroma_plane  ||  i_output_chroma == CC_ALTLINE )
        {
            /* Do an alternating line copy. This is always done for luma,
               and for 4:2:2 chroma. It can be requested for 4:2:0 chroma
               using CC_ALTLINE (see function doc).
               Note that when we get here, the number of lines matches
               in input and output.
            */
            plane_t dst_top;
            plane_t dst_bottom;
            plane_t src_top;
            plane_t src_bottom;
            FieldFromPlane( &dst_top,    &p_outpic->p[i_out_plane],   0 );
            FieldFromPlane( &dst_bottom, &p_outpic->p[i_out_plane],   1 );
            FieldFromPlane( &src_top,    &p_inpic_top->p[i_plane],    0 );
            FieldFromPlane( &src_bottom, &p_inpic_bottom->p[i_plane], 1 );
            /* Copy each field from the corresponding source. */
            plane_CopyPixels( &dst_top,    &src_top    );
            plane_CopyPixels( &dst_bottom, &src_bottom );
        }
        else /* Input 4:2:0, on a chroma plane, and not in altline mode. */
        {
            if( i_output_chroma == CC_UPCONVERT )
            {
                /* Upconverting copy - use all data from both input fields.
                   This produces an output picture with independent chroma
                   for each field. It can be used for general input when
                   the two input frames are different.
                   The output is 4:2:2, but the input is 4:2:0. Thus the output
                   has twice the lines of the input, and each full chroma plane
                   in the input corresponds to a field chroma plane in the
                   output.
                */
                plane_t dst_top;
                plane_t dst_bottom;
                FieldFromPlane( &dst_top,    &p_outpic->p[i_out_plane], 0 );
                FieldFromPlane( &dst_bottom, &p_outpic->p[i_out_plane], 1 );
                /* Copy each field from the corresponding source. */
                plane_CopyPixels( &dst_top,    &p_inpic_top->p[i_plane]    );
                plane_CopyPixels( &dst_bottom, &p_inpic_bottom->p[i_plane] );
            }
            else if( i_output_chroma == CC_SOURCE_TOP )
            {
                /* Copy chroma of input top field. Ignore chroma of input
                   bottom field. Input and output are both 4:2:0, so we just
                   copy the whole plane. */
                plane_CopyPixels( &p_outpic->p[i_out_plane],
                                  &p_inpic_top->p[i_plane] );
            }
            else if( i_output_chroma == CC_SOURCE_BOTTOM )
            {
                /* Copy chroma of input bottom field. Ignore chroma of input
                   top field. Input and output are both 4:2:0, so we just
                   copy the whole plane. */
                plane_CopyPixels( &p_outpic->p[i_out_plane],
                                  &p_inpic_bottom->p[i_plane] );
            }
            else /* i_output_chroma == CC_MERGE */
            {
                /* Average the chroma of the input fields.
                   Input and output are both 4:2:0. */
                uint8_t *p_in_top, *p_in_bottom, *p_out_end, *p_out;
                p_in_top    = p_inpic_top->p[i_plane].p_pixels;
                p_in_bottom = p_inpic_bottom->p[i_plane].p_pixels;
                p_out = p_outpic->p[i_out_plane].p_pixels;
                p_out_end = p_out + p_outpic->p[i_out_plane].i_pitch
                                  * p_outpic->p[i_out_plane].i_visible_lines;
                int w = FFMIN3( p_inpic_top->p[i_plane].i_visible_pitch,
                                p_inpic_bottom->p[i_plane].i_visible_pitch,
                                p_outpic->p[i_plane].i_visible_pitch );
                for( ; p_out < p_out_end ; )
                {
                    Merge( p_out, p_in_top, p_in_bottom, w );
                    p_out       += p_outpic->p[i_out_plane].i_pitch;
                    p_in_top    += p_inpic_top->p[i_plane].i_pitch;
                    p_in_bottom += p_inpic_bottom->p[i_plane].i_pitch;
                }
                EndMerge();
            }
        }
    }
}
