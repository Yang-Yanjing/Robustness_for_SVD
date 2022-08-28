static int parse_primary(AVExpr **e, Parser *p)
{
    AVExpr *d = av_mallocz(sizeof(AVExpr));
    char *next = p->s, *s0 = p->s;
    int ret, i;
    if (!d)
        return AVERROR(ENOMEM);
    
    d->value = av_strtod(p->s, &next);
    if (next != p->s) {
        d->type = e_value;
        p->s= next;
        *e = d;
        return 0;
    }
    d->value = 1;
    
    for (i=0; p->const_names && p->const_names[i]; i++) {
        if (strmatch(p->s, p->const_names[i])) {
            p->s+= strlen(p->const_names[i]);
            d->type = e_const;
            d->a.const_index = i;
            *e = d;
            return 0;
        }
    }
    for (i = 0; i < FF_ARRAY_ELEMS(constants); i++) {
        if (strmatch(p->s, constants[i].name)) {
            p->s += strlen(constants[i].name);
            d->type = e_value;
            d->value = constants[i].value;
            *e = d;
            return 0;
        }
    }
    p->s= strchr(p->s, '(');
    if (!p->s) {
        av_log(p, AV_LOG_ERROR, "Undefined constant or missing '(' in '%s'\n", s0);
        p->s= next;
        av_expr_free(d);
        return AVERROR(EINVAL);
    }
    p->s++; 
    if (*next == '(') { 
        av_freep(&d);
        if ((ret = parse_expr(&d, p)) < 0)
            return ret;
        if (p->s[0] != ')') {
            av_log(p, AV_LOG_ERROR, "Missing ')' in '%s'\n", s0);
            av_expr_free(d);
            return AVERROR(EINVAL);
        }
        p->s++; 
        *e = d;
        return 0;
    }
    if ((ret = parse_expr(&(d->param[0]), p)) < 0) {
        av_expr_free(d);
        return ret;
    }
    if (p->s[0]== ',') {
        p->s++; 
        parse_expr(&d->param[1], p);
    }
    if (p->s[0]== ',') {
        p->s++; 
        parse_expr(&d->param[2], p);
    }
    if (p->s[0] != ')') {
        av_log(p, AV_LOG_ERROR, "Missing ')' or too many args in '%s'\n", s0);
        av_expr_free(d);
        return AVERROR(EINVAL);
    }
    p->s++; 
    d->type = e_func0;
         if (strmatch(next, "sinh"  )) d->a.func0 = sinh;
    else if (strmatch(next, "cosh"  )) d->a.func0 = cosh;
    else if (strmatch(next, "tanh"  )) d->a.func0 = tanh;
    else if (strmatch(next, "sin"   )) d->a.func0 = sin;
    else if (strmatch(next, "cos"   )) d->a.func0 = cos;
    else if (strmatch(next, "tan"   )) d->a.func0 = tan;
    else if (strmatch(next, "atan"  )) d->a.func0 = atan;
    else if (strmatch(next, "asin"  )) d->a.func0 = asin;
    else if (strmatch(next, "acos"  )) d->a.func0 = acos;
    else if (strmatch(next, "exp"   )) d->a.func0 = exp;
    else if (strmatch(next, "log"   )) d->a.func0 = log;
    else if (strmatch(next, "abs"   )) d->a.func0 = fabs;
    else if (strmatch(next, "time"  )) d->a.func0 = etime;
    else if (strmatch(next, "squish")) d->type = e_squish;
    else if (strmatch(next, "gauss" )) d->type = e_gauss;
    else if (strmatch(next, "mod"   )) d->type = e_mod;
    else if (strmatch(next, "max"   )) d->type = e_max;
    else if (strmatch(next, "min"   )) d->type = e_min;
    else if (strmatch(next, "eq"    )) d->type = e_eq;
    else if (strmatch(next, "gte"   )) d->type = e_gte;
    else if (strmatch(next, "gt"    )) d->type = e_gt;
    else if (strmatch(next, "lte"   )) d->type = e_lte;
    else if (strmatch(next, "lt"    )) d->type = e_lt;
    else if (strmatch(next, "ld"    )) d->type = e_ld;
    else if (strmatch(next, "isnan" )) d->type = e_isnan;
    else if (strmatch(next, "isinf" )) d->type = e_isinf;
    else if (strmatch(next, "st"    )) d->type = e_st;
    else if (strmatch(next, "while" )) d->type = e_while;
    else if (strmatch(next, "taylor")) d->type = e_taylor;
    else if (strmatch(next, "root"  )) d->type = e_root;
    else if (strmatch(next, "floor" )) d->type = e_floor;
    else if (strmatch(next, "ceil"  )) d->type = e_ceil;
    else if (strmatch(next, "trunc" )) d->type = e_trunc;
    else if (strmatch(next, "sqrt"  )) d->type = e_sqrt;
    else if (strmatch(next, "not"   )) d->type = e_not;
    else if (strmatch(next, "pow"   )) d->type = e_pow;
    else if (strmatch(next, "print" )) d->type = e_print;
    else if (strmatch(next, "random")) d->type = e_random;
    else if (strmatch(next, "hypot" )) d->type = e_hypot;
    else if (strmatch(next, "gcd"   )) d->type = e_gcd;
    else if (strmatch(next, "if"    )) d->type = e_if;
    else if (strmatch(next, "ifnot" )) d->type = e_ifnot;
    else if (strmatch(next, "bitand")) d->type = e_bitand;
    else if (strmatch(next, "bitor" )) d->type = e_bitor;
    else if (strmatch(next, "between"))d->type = e_between;
    else if (strmatch(next, "clip"  )) d->type = e_clip;
    else {
        for (i=0; p->func1_names && p->func1_names[i]; i++) {
            if (strmatch(next, p->func1_names[i])) {
                d->a.func1 = p->funcs1[i];
                d->type = e_func1;
                *e = d;
                return 0;
            }
        }
        for (i=0; p->func2_names && p->func2_names[i]; i++) {
            if (strmatch(next, p->func2_names[i])) {
                d->a.func2 = p->funcs2[i];
                d->type = e_func2;
                *e = d;
                return 0;
            }
        }
        av_log(p, AV_LOG_ERROR, "Unknown function in '%s'\n", s0);
        av_expr_free(d);
        return AVERROR(EINVAL);
    }
    *e = d;
    return 0;
}
