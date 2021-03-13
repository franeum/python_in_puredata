/**
 * include the interface to Pd 
 */
#include "m_pd.h"
#include <stdio.h>
#include <Python.h>

/**
 * define a new "class" 
 */
static t_class *hworld_class;
t_float read_script(t_float f);

/**
 * this is the dataspace of our new object
 * we don't need to store anything,
 * however the first (and only) entry in this struct
 * is mandatory and of type "t_object"
 */
typedef struct _hworld {
  t_object  x_obj;
} t_hworld;

/*
double call_func(PyObject *func, double x, double y) 
{ 
    PyObject *args; 
    PyObject *kwargs; 
    PyObject *result = 0; 
    double retval; 
      
    // Make sure we own the GIL 
    PyGILState_STATE state = PyGILState_Ensure(); 
      
      
    // Verify that func is a proper callable 
    if (!PyCallable_Check(func)) 
    { 
        fprintf(stderr, "call_func: expected a callable\n"); 
        goto fail; 
    } 

    // Step3 
    args = Py_BuildValue("(dd)", x, y); 
    kwargs = NULL; 
      
    // Step 4 
    result = PyObject_Call(func, args, kwargs); 
    Py_DECREF(args); 
    Py_XDECREF(kwargs); 
      
    // Step 5 
    if (PyErr_Occurred()) 
    { 
        PyErr_Print(); 
        goto fail; 
    } 
      
    // Verify the result is a float object  
    if (!PyFloat_Check(result)) 
    { 
        fprintf(stderr, "call_func: callable didn't return a float\n"); 
        goto fail; 
    } 
      
    // Step 6 
    retval = PyFloat_AsDouble(result); 
    Py_DECREF(result); 
      
    // Step 7 
    PyGILState_Release(state); 
    return retval; 
    fail: 
        Py_XDECREF(result); 
        PyGILState_Release(state); 
        abort();  
} 



PyObject *import_name(const char *modname, const char *symbol) 
{ 
    PyObject *u_name, *module; 
    u_name = PyUnicode_FromString(modname); 
    module = PyImport_Import(u_name); 
    Py_DECREF(u_name); 
      
    return PyObject_GetAttrString(module, symbol); 
} 

*/
/**
 * this method is called whenever a "bang" is sent to the object
 * the name of this function is arbitrary and is registered to Pd in the 
 * hworld_setup() routine
 */

/*
void hworld_bang(t_hworld *x)
{
    PyObject * pow_func; 
    
    Py_Initialize(); 
      
    // Get a reference to the math.pow function 
    pow_func = import_name("simple", "simplest"); 
      
    // Call it using our call_func() code  
    // post("10 %0.2f\n", call_func(pow_func, 10, 2.0)); 

    double res = call_func(pow_func, 10, 2.0);
          
    Py_DECREF(pow_func); 
    Py_Finalize(); 
    //post("Hello world !!");

    outlet_float(x->x_obj.ob_outlet, res);
}
*/






void hworld_bang(t_hworld *x)
{
    //double res = read_script(10.0);
    outlet_float(x->x_obj.ob_outlet, read_script(10.0));
}




void hworld_float(t_hworld *x, t_floatarg f)
{
    outlet_float(x->x_obj.ob_outlet, read_script(f));    
}





t_float read_script(t_float f)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    //int i;
    t_float result = 0.0;

    Py_Initialize();
    //pName = PyUnicode_DecodeFSDefault(argv[1]);
    /* Error checking of pName left out */


    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/home/neum/Documenti/python_in_puredata\")");


    pName = PyUnicode_DecodeFSDefault("thesimple");
    pModule = PyImport_Import(pName);

    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "simplest");
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            /*
            pArgs = PyTuple_New(argc - 3);
            for (i = 0; i < argc - 3; ++i) {
                pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                // pValue reference stolen here: 
                PyTuple_SetItem(pArgs, i, pValue);
            }
            */
            pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, PyFloat_FromDouble(f));
            //pArgs = PyFloat_FromDouble(10.0);

            //post("value: %f", pArgs);

            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                //printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                result = (t_float)PyLong_AsLong(pValue);
                Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                post("Call failed");
                return 1.0;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            post("Cannot find function");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        post("Failed to load");
        return 1.0;
    }


    if (Py_FinalizeEx() < 0) {
        return 120.0;
    }
    
    
    return result;
}














/**
 * this is the "constructor" of the class
 * this method is called whenever a new object of this class is created
 * the name of this function is arbitrary and is registered to Pd in the 
 * hworld_setup() routine
 */
void *hworld_new(void)
{
  /*
   * call the "constructor" of the parent-class
   * this will reserve enough memory to hold "t_hworld"
   */
  t_hworld *x = (t_hworld *)pd_new(hworld_class);

  outlet_new(&x->x_obj, &s_float);

  /*
   * return the pointer to the class - this is mandatory
   * if you return "0", then the object-creation will fail
   */
  return (void *)x;
}


/**
 * define the function-space of the class
 * within a single-object external the name of this function is special
 */
void hworld_setup(void) {
  /* create a new class */
  hworld_class = class_new(gensym("hworld"),        /* the object's name is "hworld" */
			       (t_newmethod)hworld_new, /* the object's constructor is "hworld_new()" */
			       0,                           /* no special destructor */
			       sizeof(t_hworld),        /* the size of the data-space */
			       CLASS_DEFAULT,               /* a normal pd object */
			       0);                          /* no creation arguments */

  /* attach functions to messages */
  /* here we bind the "hworld_bang()" function to the class "hworld_class()" -
   * it will be called whenever a bang is received
   */
  class_addbang(hworld_class, hworld_bang); 
  class_addfloat(hworld_class, hworld_float);
}