/**
 * include the interface to Pd
 */
#include "m_pd.h"
#include <stdio.h>
// #include <Python.h>

/**
 * define a new "class"
 */
static t_class *tarray_class;

typedef struct _tarray
{
    t_object x_obj;
    t_symbol *arrayname;
} t_tarray;

// t_float read_script(t_tarray *x, t_symbol *module, t_float f);

void tarray_bang(t_tarray *x)
{
    t_garray *a;
    int npoints;
    t_word *vec;

    if (!(a = (t_garray *)pd_findbyclass(x->arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->arrayname->s_name);
    else if (!garray_getfloatwords(a, &npoints, &vec))
        pd_error(x, "%s: bad template for tabread", x->arrayname->s_name);
    else
    {
        /*
        int n = f;
        if (n < 0) n = 0;
        else if (n >= npoints) n = npoints - 1;
        outlet_float(x->x_obj.ob_outlet, (npoints ? vec[n].w_float : 0));
        */

        // outlet_list(x->x_obj.ob_outlet, &s_list, 2, start);
        float previous = 2.;

        for (int i = 0; i < npoints; i++)
        {
            // vec[i].w_float /= 2.0;
            float current = vec[i].w_float;
            if (previous < 0.0 && current >= 0.0)
            {
                outlet_float(x->x_obj.ob_outlet, i);
            }
            previous = current;
        }

        // garray_redraw(a);
        //  t_atom stop[2];

        // outlet_list(x->x_obj.ob_outlet, &s_list, 2, stop);
    }
}

void tarray_float(t_tarray *x, t_floatarg f)
{
    // outlet_float(x->x_obj.ob_outlet, read_script(f));
    ;
}

void tarray_list(t_tarray *x, t_symbol *s, int argc, t_atom *argv)
{
    post(s->s_name);
}

void tarray_anything(t_tarray *x, t_symbol *s, int argc, t_atom *argv)
{
    post("funcname: %s", s->s_name);
    postatom(argc, argv);
    t_float arg = atom_getfloat(&argv[0]);
    post("\n");
}

void tarray_free(t_tarray *x)
{
    ;
}

void *tarray_new(t_symbol *s)
{
    t_tarray *x = (t_tarray *)pd_new(tarray_class);

    x->arrayname = s;

    outlet_new(&x->x_obj, &s_float);

    /*
     * return the pointer to the class - this is mandatory
     * if you return "0", then the object-creation will fail
     */
    return (void *)x;
}

extern "C" {
    void tarray_setup(void)
    {
        /* create a new class */
        tarray_class = class_new(gensym("tarray"),        /* the object's name is "py" */
                                    (t_newmethod)tarray_new, /* the object's constructor is "tarray_new()" */
                                    (t_method)tarray_free,   /* no special destructor */
                                    sizeof(t_tarray),        /* the size of the data-space */
                                    0,
                                    A_DEFSYM,
                                    0); /* no creation arguments */

        class_addbang(tarray_class, tarray_bang);
        class_addfloat(tarray_class, tarray_float);
        class_addlist(tarray_class, tarray_list);
        class_addanything(tarray_class, tarray_anything);
    }
}