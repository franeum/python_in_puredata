/**
 * include the interface to Pd 
 */
#include "m_pd.h"
#include <stdio.h>
#include <Python.h>

/**
 * define a new "class" 
 */
static t_class *pyaddlist_class;

typedef struct _pyaddlist
{
    t_object x_obj;
    t_symbol *mModule;
} t_pyaddlist;

t_float read_script(t_pyaddlist *x, t_symbol *funcname, int argc, t_atom *argv);

void pyaddlist_bang(t_pyaddlist *x)
{
    //double res = read_script(10.0);
    //outlet_float(x->x_obj.ob_outlet, read_script(10.0));
    ;
}

void pyaddlist_float(t_pyaddlist *x, t_floatarg f)
{
    //outlet_float(x->x_obj.ob_outlet, read_script(f));
    ;
}

void pyaddlist_list(t_pyaddlist *x, t_symbol *s, int argc, t_atom *argv)
{
    post(s->s_name);
}

void pyaddlist_anything(t_pyaddlist *x, t_symbol *s, int argc, t_atom *argv)
{
    post("funcname: %s", s->s_name);
    postatom(argc, argv);
    t_float arg = atom_getfloat(&argv[0]);
    post("\n");

    //if (argc)
    //arg = atom_getfloat(&argv[0]);

    //outlet_float(x->x_obj.ob_outlet, read_script(x, s, arg));
    read_script(x, s, argc, argv);
}

void pyaddlist_import(t_pyaddlist *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *module = atom_getsymbol(&argv[0]);
    post("module name: %s", module->s_name);
    x->mModule = module;
}

/*
void pyaddlist_func(t_pyaddlist *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *funcname = atom_getsymbol(&argv[0]);
    t_float arg = atom_getfloat(&argv[1]);

    //outlet_float(x->x_obj.ob_outlet, read_script(x, funcname, arg));
    read_script(x, funcname, arg);
}
*/

void pyaddlist_free(t_pyaddlist *x)
{
    Py_FinalizeEx();
}

t_float read_script(t_pyaddlist *x, t_symbol *funcname, int argc, t_atom *argv)
{
    PyObject *pName, *pModule, *pFunc, *pList;
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
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc))
        {

            if (argc)
            {
                pList = PyList_New(argc);
                pArgs = PyTuple_New(1);

                for (int i = 0; i < argc; i++)
                {
                    post("%d", (t_int)atom_getfloat(&argv[i]));
                    PyList_SetItem(pList, i, PyLong_FromLong((t_int)atom_getfloat(&argv[i])));
                }

                post("lunghezza lista: %d", PyList_Size(pList));

                PyTuple_SetItem(pArgs, 0, pList);
                pValue = PyObject_CallObject(pFunc, pArgs);
                Py_DECREF(pArgs);
            }
            else
            {
                pArgs = NULL;
                pValue = PyObject_CallObject(pFunc, pArgs);
            }

            if (pValue != NULL)
            {

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
    if (pyaddlist_FinalizeEx() < 0) {
        return 120.0;
    }
    */

    return result;
}

void *pyaddlist_new(void)
{
    t_pyaddlist *x = (t_pyaddlist *)pd_new(pyaddlist_class);

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/home/neum/Documenti/python_in_puredata/esperimento5\")");
    PyRun_SimpleString("print('python version is:', sys.version)");
    PyRun_SimpleString("print(sys.prefix)");
    wchar_t *vedoma = Py_GetProgramName();

    post("%ls", vedoma);
    post("%ls", Py_GetExecPrefix());
    post("%ls", Py_GetProgramFullPath());
    post("%s", Py_GetPlatform());
    size_t *sz;
    *sz = 2;
    Py_SetPythonHome(Py_DecodeLocale("./", sz));
    post("%ls", Py_GetPythonHome());
    outlet_new(&x->x_obj, &s_float);

    /*
    * return the pointer to the class - this is mandatory
    * if you return "0", then the object-creation will fail
    */
    return (void *)x;
}

void pyaddlist_setup(void)
{
    /* create a new class */
    pyaddlist_class = class_new(gensym("pyaddlist"),        /* the object's name is "py" */
                                (t_newmethod)pyaddlist_new, /* the object's constructor is "pyaddlist_new()" */
                                (t_method)pyaddlist_free,   /* no special destructor */
                                sizeof(t_pyaddlist),        /* the size of the data-space */
                                CLASS_DEFAULT,              /* a normal pd object */
                                0);                         /* no creation arguments */

    class_addbang(pyaddlist_class, pyaddlist_bang);
    class_addfloat(pyaddlist_class, pyaddlist_float);
    class_addlist(pyaddlist_class, pyaddlist_list);
    class_addanything(pyaddlist_class, pyaddlist_anything);
    //class_addmethod(pyaddlist_class, (t_method)pyaddlist_func, gensym("func"), A_GIMME, 0);
    class_addmethod(pyaddlist_class, (t_method)pyaddlist_import, gensym("import"), A_GIMME, 0);
}