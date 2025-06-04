#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *handle;
    void (*hello_func)();

    // Load the shared library
    handle = dlopen("./libexample.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    // Clear existing errors
    dlerror();

    // Get the function
    hello_func = dlsym(handle, "mod_init");
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Error: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // Call the function
    hello_func();

    // Close the library
    dlclose(handle);
    return 0;
}
