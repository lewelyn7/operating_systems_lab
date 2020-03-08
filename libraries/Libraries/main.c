//functions for dll library
#include <dlfcn.h>

#ifndef DLL
//DLL library doesn't need this header file
#include "say.h"
#endif

int main() {
#ifdef DLL
//open library
//function returns library handler
//takes dynamic library path and a flag
void *handle = dlopen("./libsay.so", RTLD_LAZY);

//now - pointers to all used functions
//function dlsym takes library handler and function name, returns function pointer
void (*sayHello)(void) = dlsym(handle, "sayHello");
int (*say)(char*) = dlsym(handle, "say");
//ok, we can use functions from our DLL library
#endif

  sayHello();
  int res = say("world");
  return res;

#ifdef DLL
  //close library after doing all stuff
  dlclose(handle);
#endif
}
