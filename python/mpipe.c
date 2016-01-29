/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <python2.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
PyObject *py_writepipe(PyObject* self, PyObject* args)
{
  int x,size,res;
  char * content;
  if (!PyArg_ParseTuple(args, "isi", &x,content,&size))
    return NULL;
  if (content != NULL)
    res = write(x,content,size);
  return Py_BuildValue("i", res);
}
static PyMethodDef Module_methods[] = {
  {"writepipe", py_writepipe, METH_VARARGS,"writepipe"},
  {NULL, NULL}
};
void initmpipe()
{
  PyObject* m;
  m = Py_InitModule("mpipe", Module_methods);
}
