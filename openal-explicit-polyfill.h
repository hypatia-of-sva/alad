#include "alad.h"

#ifndef OPENAL_EXPLICIT_POLYFILL
#define OPENAL_EXPLICIT_POLYFILL


/* to separate listener- and context-dependency */
typedef ALlistener *ALCdevice;



/*
 * Logic by which device, context or listener is added or not:
 * a) any function taking in a buffer id needs not to take a device, but might need a listener or context
 * b) any function taking in a source id needs not to take a device or context, but might need a listener
 * c) any function takinn a context needs not to take a device
 * c) any function takinn a listener needs not to take a device or context
 *    since they carry information about them by the creation of the id / context.
 * When Allistener and when ALCcontext* should be used, however, is not quite clear since it is currently the same
 * The idea behind this is to potentially allow multiple listeners operating on the same sources.
 * Therefore, the functions changing the listener state (like its position and direction) should take Allistener,
 * while the functions changing global context state (like Enable or DistanceModel) should take ALCcontext*.
 */



/* Function prototypes for added context references: */
void            alDopplerFactorExplicitEXT      (ALCcontext* context, ALfloat value);
void            alDopplerVelocityExplicitEXT    (ALCcontext* context, ALfloat value);
void            alSpeedOfSoundExplicitEXT       (ALCcontext* context, ALfloat value);
void            alDistanceModelExplicitEXT      (ALCcontext* context, ALenum distanceModel);
void            alEnableExplicitEXT             (ALCcontext* context, ALenum capability);
void            alDisableExplicitEXT            (ALCcontext* context, ALenum capability);
ALboolean       alIsEnabledExplicitEXT          (ALCcontext* context, ALenum capability);
const ALchar*   alGetStringExplicitEXT          (ALCcontext* context, ALenum param);
void            alGetBooleanvExplicitEXT        (ALCcontext* context, ALenum param, ALboolean *values);
void            alGetIntegervExplicitEXT        (ALCcontext* context, ALenum param, ALint *values);
void            alGetFloatvExplicitEXT          (ALCcontext* context, ALenum param, ALfloat *values);
void            alGetDoublevExplicitEXT         (ALCcontext* context, ALenum param, ALdouble *values);
ALboolean       alGetBooleanExplicitEXT         (ALCcontext* context, ALenum param);
ALint           alGetIntegerExplicitEXT         (ALCcontext* context, ALenum param);
ALfloat         alGetFloatExplicitEXT           (ALCcontext* context, ALenum param);
ALdouble        alGetDoubleExplicitEXT          (ALCcontext* context, ALenum param);
ALenum          alGetErrorExplicitEXT           (ALCcontext* context, void);
ALboolean       alIsExtensionPresentExplicitEXT (ALCcontext* context, const ALchar *extname);
void*           alGetProcAddressExplicitEXT     (ALCcontext* context, const ALchar *fname);
ALenum          alGetEnumValueExplicitEXT       (ALCcontext* context, const ALchar *ename);



/* New functions that currently do basically nothing, but could be expanded upon */
ALlistener      alCreateListenerEXT             (ALCcontext* context);
void            alDestroyListenerEXT            (ALCcontext* context, ALlistener listener);



#ifdef OPENAL_EXPLICIT_POLYFILL_IMPLEMENTATION





ALlistener      alCreateListenerEXT             (ALCcontext* context) {
  return context;
}
void            alDestroyListenerEXT            (ALCcontext* context, ALlistener listener) {
   /* assert(context == listener); */ /* currently not needed, but could perform this check among other things */
   (void)0;
}

#endif /* OPENAL_EXPLICIT_POLYFILL_IMPLEMENTATION */

#endif /* OPENAL_EXPLICIT_POLYFILL */
