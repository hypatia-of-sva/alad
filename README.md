# alad

glad-like, header-only C99 OpenAL-loader library by Hypatia of Sva

Usage:

Include the single header

        #include "alad.h"

This will pull in all OpenAL symbols except the functions. Initalize those with

        aladLoadAL(NULL);

for default initialization, or with

        aladLoadAL(my_alGetProcAddress);

where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization.
Default initialization will pull in "OpenAL32.dll" or "soft_oal.dll" on Windows and "libopenal.so" or "libopenal.so.1" on Unix respectively.
Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code in the internal function "_alad_open" or provide your own function loader.

Update ALC pointers to those loaded with a specific ALCdevice with

        aladUpdateALCPointers(device);

Unload the library loaded with default initalization with

        aladTerminate();


alad will attempt to load all function pointers in all extensions, the ones not available will be NULL, you have to check all functions you want to use for that.
The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
If a functionality you expect to be present isn't working, you might want to check the spelling of the string in the internal "_alad_load_alc_functions" function.
