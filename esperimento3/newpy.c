/**
 * include the interface to Pd 
 */
#include "m_pd.h"
#include <stdio.h>
#include <Python.h>

/**
 * define a new "class" 
 */
static t_class *newpy_class;

typedef struct _newpy
{
    t_object x_obj;
    t_symbol *mModule;
} t_newpy;

t_float read_script(t_newpy *x, t_symbol *module, t_float f);

void newpy_bang(t_newpy *x)
{
    //double res = read_script(10.0);
    //outlet_float(x->x_obj.ob_outlet, read_script(10.0));
    ;
}

void newpy_float(t_newpy *x, t_floatarg f)
{
    //outlet_float(x->x_obj.ob_outlet, read_script(f));
    ;
}

void newpy_list(t_newpy *x, t_symbol *s, int argc, t_atom *argv)
{
    post(s->s_name);
}

void newpy_anything(t_newpy *x, t_symbol *s, int argc, t_atom *argv)
{
    post("funcname: %s", s->s_name);
    postatom(argc, argv);
    t_float arg = atom_getfloat(&argv[0]);
    post("\n");

    //if (argc)
    //arg = atom_getfloat(&argv[0]);

    outlet_float(x->x_obj.ob_outlet, read_script(x, s, arg));
}

void newpy_import(t_newpy *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *module = atom_getsymbol(&argv[0]);
    post("module name: %s", module->s_name);
    x->mModule = module;
}

void newpy_func(t_newpy *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *funcname = atom_getsymbol(&argv[0]);
    t_float arg = atom_getfloat(&argv[1]);

    //outlet_float(x->x_obj.ob_outlet, read_script(x, funcname, arg));
    read_script(x, funcname, arg);
}

void newpy_free(t_newpy *x)
{
    Py_FinalizeEx();
}

t_float read_script(t_newpy *x, t_symbol *funcname, t_float f)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    //int i;
    t_float result;

    if (x->mModule != NULL)
    {
        pName = PyUnicode_DecodeFSDefault(x->mModule->s_name);
        pModule = PyImport_Import(pName);
    }
    else
    {
        post("No module imported, import a module with import statement");
        return -1;
    }

    Py_DECREF(pName);

    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, funcname->s_name);
        //pFunc = PyUnicode_DecodeFSDefault(funcname->s_name);
        //pFunc = PyUnicode_FromString(funcname->s_name);

        //PyObject *moduleMainString = PyUnicode_FromString("__main__");
        //PyObject *moduleMain = PyImport_Import(moduleMainString);

        //pFunc = newPy_BuildValue(funcname->s_name);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc))
        {

            if (f)
            {
                pArgs = PyTuple_New(1);
                //PyTuple_SetItem(pArgs, 0, PyFloat_FromDouble(f));
                PyTuple_SetItem(pArgs, 0, PyLong_FromLong((t_int)f));
                pValue = PyObject_CallObject(pFunc, pArgs);
                Py_DECREF(pArgs);
            }
            else
            {
                pArgs = NULL;
                pValue = PyObject_CallObject(pFunc, pArgs);
            }

            //pArgs = PyFloat_FromDouble(10.0);

            //post("value: %f", pArgs);

            //pValue = PyObject_CallObject(pFunc, pArgs);
            //newPy_DECREF(pArgs);

            if (pValue != NULL)
            {
                //printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                //result = (t_float)PyFloat_AsDouble(pValue);
                //result = (t_float)PyList_Size(pValue);
                int size = (t_int)PyList_Size(pValue);

                //outlet_float(x->x_obj.ob_outlet, read_script(x, funcname, arg));
                //outlet_list(x->x_obj.ob_outlet, &s_list, size);

                t_float arr[size];
                t_atom tastoma[size];

                //SETFLOAT(&x->result[i], x->l_list[i].a_w.w_float / x->r_list[i].a_w.w_float);

                for (int i = 0; i < size; i++)
                {
                    t_float item = PyFloat_AsDouble(PyList_GetItem(pValue, i));
                    SETFLOAT(&tastoma[i], item);
                }

                //t_atom *tastoma;

                Py_DECREF(pValue);
                outlet_list(x->x_obj.ob_outlet, &s_list, size, tastoma);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                //post(pValue);
                post("Call failed");
                return 1.0;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            post("Cannot find function");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        post("Failed to load module");
        return 1.0;
    }

    /*
    if (newPy_FinalizeEx() < 0) {
        return 120.0;
    }
    */

    return result;
}

void *newpy_new(void)
{
    t_newpy *x = (t_newpy *)pd_new(newpy_class);

    Py_Initialize();
    //PyRun_SimpleString("import sys");
    //PyRun_SimpleString("sys.path.append(\"/home/neum/Documenti/python_in_puredata\")");
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/home/neum/Documenti/python_in_puredata/esperimento3\")");

    outlet_new(&x->x_obj, &s_float);

    /*
    * return the pointer to the class - this is mandatory
    * if you return "0", then the object-creation will fail
    */
    return (void *)x;
}

void newpy_setup(void)
{
    /* create a new class */
    newpy_class = class_new(gensym("py"),           /* the object's name is "py" */
                            (t_newmethod)newpy_new, /* the object's constructor is "newpy_new()" */
                            (t_method)newpy_free,   /* no special destructor */
                            sizeof(t_newpy),        /* the size of the data-space */
                            CLASS_DEFAULT,          /* a normal pd object */
                            0);                     /* no creation arguments */

    class_addbang(newpy_class, newpy_bang);
    class_addfloat(newpy_class, newpy_float);
    class_addlist(newpy_class, newpy_list);
    class_addanything(newpy_class, newpy_anything);
    class_addmethod(newpy_class, (t_method)newpy_func, gensym("func"), A_GIMME, 0);
    class_addmethod(newpy_class, (t_method)newpy_import, gensym("import"), A_GIMME, 0);
}