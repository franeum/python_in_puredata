/**
 * include the interface to Pd
 */
#include "m_pd.h"
#include <stdio.h>
// #include <Python.h>

/**
 * define a new "class"
 */
static t_class *tastomarray_class;

typedef struct _tastomarray
{
    t_object x_obj;
    t_symbol *arrayname;
} t_tastomarray;

// t_float read_script(t_tastomarray *x, t_symbol *module, t_float f);

void tastomarray_bang(t_tastomarray *x)
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

void tastomarray_float(t_tastomarray *x, t_floatarg f)
{
    // outlet_float(x->x_obj.ob_outlet, read_script(f));
    ;
}

void tastomarray_list(t_tastomarray *x, t_symbol *s, int argc, t_atom *argv)
{
    post(s->s_name);
}

void tastomarray_anything(t_tastomarray *x, t_symbol *s, int argc, t_atom *argv)
{
    post("funcname: %s", s->s_name);
    postatom(argc, argv);
    t_float arg = atom_getfloat(&argv[0]);
    post("\n");
}

void tastomarray_free(t_tastomarray *x)
{
    ;
}

void *tastomarray_new(t_symbol *s)
{
    t_tastomarray *x = (t_tastomarray *)pd_new(tastomarray_class);

    x->arrayname = s;

    outlet_new(&x->x_obj, &s_float);

    /*
     * return the pointer to the class - this is mandatory
     * if you return "0", then the object-creation will fail
     */
    return (void *)x;
}

void tastomarray_setup(void)
{
    /* create a new class */
    tastomarray_class = class_new(gensym("tastomarray"),        /* the object's name is "py" */
                                  (t_newmethod)tastomarray_new, /* the object's constructor is "tastomarray_new()" */
                                  (t_method)tastomarray_free,   /* no special destructor */
                                  sizeof(t_tastomarray),        /* the size of the data-space */
                                  0,
                                  A_DEFSYM,
                                  0); /* no creation arguments */

    class_addbang(tastomarray_class, tastomarray_bang);
    class_addfloat(tastomarray_class, tastomarray_float);
    class_addlist(tastomarray_class, tastomarray_list);
    class_addanything(tastomarray_class, tastomarray_anything);
}