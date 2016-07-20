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
#include "string.h"
PyObject *py_writepipe(PyObject* self, PyObject* args)
{
  int x,size,res;
  char *content;
  if (!PyArg_ParseTuple(args, "is", &x,&content))
    return NULL;
  size = strlen(content)+1;//add \0 on the end
//  printf("%d,%s,%d\n",x,content,size);
  if (content != NULL && size != 0)
    res = write(x,content,size);
  return Py_BuildValue("i", res);
}
PyObject *py_readpipe(PyObject* self, PyObject* args)
{
  int x;
  char content[2048];
  if (!PyArg_ParseTuple(args, "i", &x))
    return NULL;
  int tmp;
  for(tmp=0;tmp<2048;tmp++){
    char tc[1];
    if(read(x,tc,1)!=1){//error
      printf("Error: mpipe.c: read(x,tc,1)!=1\n");
      return (PyObject *)0;
    }
    content[tmp] = tc[0];
    if(tc[0]=='\0'){//one line
      return Py_BuildValue("s", content);
    }
  }
  printf("Error: mpipe.c: read buffer overflow.\n");
  return Py_BuildValue("s", content);
}
static PyMethodDef Module_methods[] = {
  {"writepipe", py_writepipe, METH_VARARGS,"writepipe"},
  {"readpipe", py_readpipe, METH_VARARGS,"readpipe"},
  {NULL, NULL}
};
void initmpipe()
{
  PyObject* m;
  m = Py_InitModule("mpipe", Module_methods);
}
