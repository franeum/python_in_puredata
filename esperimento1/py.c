/**
 * include the interface to Pd 
 */
#include "m_pd.h"
#include <stdio.h>
#include <Python.h>

/**
 * define a new "class" 
 */
static t_class *py_class;


typedef struct _py {
    t_object  x_obj;
    t_symbol *mModule;
} t_py;

t_float read_script(t_py *x, t_symbol *module, t_float f);



void py_bang(t_py *x)
{
    //double res = read_script(10.0);
    //outlet_float(x->x_obj.ob_outlet, read_script(10.0));
    ;
}




void py_float(t_py *x, t_floatarg f)
{
    //outlet_float(x->x_obj.ob_outlet, read_script(f));  
    ;  
}



void py_list(t_py *x, t_symbol *s, int argc, t_atom *argv)
{
    post(s->s_name);
}



void py_anything(t_py *x, t_symbol *s, int argc, t_atom *argv)
{
    post("funcname: %s", s->s_name);
    t_float arg = atom_getfloat(&argv[0]);

    //if (argc)
        //arg = atom_getfloat(&argv[0]);

    outlet_float(x->x_obj.ob_outlet, read_script(x, s, arg));
}


void py_import(t_py *x, t_symbol *s, int argc, t_atom *argv) 
{
    t_symbol *module = atom_getsymbol(&argv[0]);
    post("module name: %s", module->s_name);
    x->mModule = module;
}


void py_func(t_py *x, t_symbol *s, int argc, t_atom *argv) 
{
    t_symbol *funcname = atom_getsymbol(&argv[0]);
    t_float arg = atom_getfloat(&argv[1]);
    outlet_float(x->x_obj.ob_outlet, read_script(x, funcname, arg));
}


void py_free(t_py *x) 
{
    Py_FinalizeEx();    
}



t_float read_script(t_py *x, t_symbol *funcname, t_float f)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    //int i;
    t_float result;

    if (x->mModule != NULL) {
        pName = PyUnicode_DecodeFSDefault(x->mModule->s_name);
        pModule = PyImport_Import(pName);
    } else {
        post("No module imported, import a module with import statement");
        return -1;
    }

    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, funcname->s_name);
        //pFunc = PyUnicode_DecodeFSDefault(funcname->s_name);
        //pFunc = PyUnicode_FromString(funcname->s_name);

        //PyObject *moduleMainString = PyUnicode_FromString("__main__");
        //PyObject *moduleMain = PyImport_Import(moduleMainString);

        //pFunc = Py_BuildValue(funcname->s_name);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            
            if (f) {
                pArgs = PyTuple_New(1);
                PyTuple_SetItem(pArgs, 0, PyFloat_FromDouble(f));
                pValue = PyObject_CallObject(pFunc, pArgs);
                Py_DECREF(pArgs);
            } else {
                pArgs = NULL;
                pValue = PyObject_CallObject(pFunc, pArgs);
            }

            //pArgs = PyFloat_FromDouble(10.0);

            //post("value: %f", pArgs);

            //pValue = PyObject_CallObject(pFunc, pArgs);
            //Py_DECREF(pArgs);

            if (pValue != NULL) {
                //printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                result = (t_float)PyFloat_AsDouble(pValue);
                Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                post("Call failed");
                return 1.0;
            }
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            post("Cannot find function");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        post("Failed to load module");
        return 1.0;
    }

    /*
    if (Py_FinalizeEx() < 0) {
        return 120.0;
    }
    */
    
    
    return result;
}




void *py_new(void)
{
    t_py *x = (t_py *)pd_new(py_class);

    Py_Initialize();
    //PyRun_SimpleString("import sys");
    //PyRun_SimpleString("sys.path.append(\"/home/neum/Documenti/python_in_puredata\")");

    outlet_new(&x->x_obj, &s_float);

    /*
    * return the pointer to the class - this is mandatory
    * if you return "0", then the object-creation will fail
    */
    return (void *)x;
}


void py_setup(void) {
  /* create a new class */
    py_class = class_new(gensym("py"),        /* the object's name is "py" */
                    (t_newmethod)py_new, /* the object's constructor is "py_new()" */
                    (t_method)py_free,                           /* no special destructor */
                    sizeof(t_py),        /* the size of the data-space */
                    CLASS_DEFAULT,               /* a normal pd object */
                    0);                          /* no creation arguments */


    class_addbang(py_class, py_bang); 
    class_addfloat(py_class, py_float);
    class_addlist(py_class, py_list);
    class_addanything(py_class, py_anything);
    class_addmethod(py_class, (t_method)py_func, gensym("func"), A_GIMME, 0);
    class_addmethod(py_class, (t_method)py_import, gensym("import"), A_GIMME, 0);

}