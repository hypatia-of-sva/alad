# alad

alad is a [glad](https://github.com/Dav1dde/glad)-like, header-only C99 [OpenAL](http://www.openal.org/)-loader library by Hypatia of Sva

Usage:

Include the single header

        #include "alad.h"
        
once with

        #define ALAD_IMPLEMENTATION

to define the functions and global function pointers, and include the header without this symbol defined in all other translation units.

This will pull in all OpenAL symbols except the functions. Initalize those with

        aladLoadAL(NULL);

for default initialization, or with

        aladLoadAL(my_alGetProcAddress);

where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization, to load all the function via alGetProcAddress.

Default initialization will pull in "OpenAL32.dll" or "soft_oal.dll" on Windows and "libopenal.so" or "libopenal.so.1" on Unix respectively.

Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code in the internal function "_alad_open" or provide your own function loader.

The shared library will only be loaded once, you can call "aladLoadAL(NULL)" as often as you want to reload the pointers from the loaded shared library.


If you're unsure about loading with a function loader, intialize with

        aladLoadALContextFree();

to load function pointers directly from the shared library. This only works for default initialization.
This will however not load all function pointers, but only those necessary to create a context.
Those are the core ALC functions and all Core AL functions not relating to buffers, listeners, sources,
and also not alDopplerFactor, alDopplerVelocity, alSpeedOfSound and alDistanceModel.

If you want to use your own library, not available in default initialization, and want to load directly from it, you need to write a wrapper of type
LPALGETPROCADDRESS of the form

        my_alGetProcAddress("[al-function]") = dlsym(my_lib, "[al-function]").
        
aladLoadALContextFree is not used here.

These aladLoad functions only initialize the Core API. To load the extensions you have to update the function pointers.

You can update those via the ALCcontext* context through

        aladUpdateALPointers(context);
        aladUpdateALCPointersFromContext(context);

which will load all AL and ALC functions, including extensions, via alGetProcAddress for the specific context.

Update ALC pointers to those loaded with a specific ALCdevice* device with

        aladUpdateALCPointersFromDevice(device);

If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladUpdateALCPointersFromContext" again.
or by calling "aladLoadALContextFree()" if you need them without reference to any context.
Calling "aladLoadAL" again won't do anything different from "aladUpdateALCPointersFromContext",
since both call alGetProcAddress and are therefore just dependent on driver state.


Update ALC pointers to those loaded with a specific ALCdevice with

        aladUpdateALCPointers(device);

If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladLoadAL" again, or by calling "aladUpdateALCPointers(NULL);"

Unload the library loaded with default initalization with

        aladTerminate();
        
This will not unload the function pointers or set them to NULL, but they will be probably invalid after unloading the shared library.


alad will attempt to load all function pointers in all extensions as currently described in the [openal-soft](https://github.com/kcat/openal-soft) headers (repo version from 24.12.2022, commit https://github.com/kcat/openal-soft/commit/4fe6eba8c79a4c9cad91d6f6835506cde96a48c4), the ones not available will be NULL, you have to check all functions you want to use for that.
The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
If a functionality you expect to be present isn't working, you might want to check the spelling of the strings in the internal "_alad_load_alc_functions" function.

Dependencies for compilation: Kernel32.dll on Windows and -ldl on Unix for the default initialization.
