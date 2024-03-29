/*
 * "The Road goes ever on and on, down from the door where it began."
 */

#include "PerlInterpreter.h"
#include <dlfcn.h>

#include "EXTERN.h"
#include "perl.h"

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

static void xs_init _((void));
static PerlInterpreter *my_perl;

int jpldebug = 0;
JNIEnv *jplcurenv;

JNIEXPORT void JNICALL
Java_PerlInterpreter_init(JNIEnv *env, jobject obj, jstring js)
{
    int exitstatus;
    int argc = 3;
    SV* envsv;
    SV* objsv;
 
    static char *argv[] = {"perl", "-e", "1", 0};

    if (getenv("JPLDEBUG"))
	jpldebug = atoi(getenv("JPLDEBUG"));

    if (jpldebug)
	fprintf(stderr, "init\n");

    if (!dlopen("libperl.so", RTLD_LAZY|RTLD_GLOBAL)) {
	fprintf(stderr, "%s\n", dlerror());
	exit(1);
    }

    if (PL_curinterp)
	return;

    perl_init_i18nl10n(1);

    if (!PL_do_undump) {
	my_perl = perl_alloc();
	if (!my_perl)
	    exit(1);
	perl_construct( my_perl );
	PL_perl_destruct_level = 0;
    }

    exitstatus = perl_parse( my_perl, xs_init, argc, argv, (char **) NULL );
    
    if (!exitstatus)
	Java_PerlInterpreter_eval(env, obj, js);

}

JNIEXPORT void JNICALL
Java_PerlInterpreter_eval(JNIEnv *env, jobject obj, jstring js)
{
    SV* envsv;
    SV* objsv;
    dSP;
    jbyte* jb;

    ENTER;
    SAVETMPS;

    jplcurenv = env;
    envsv = perl_get_sv("JPL::_env_", 1);
    sv_setiv(envsv, (IV)(void*)env);
    objsv = perl_get_sv("JPL::_obj_", 1);
    sv_setiv(objsv, (IV)(void*)obj);

    jb = (jbyte*)(*env)->GetStringUTFChars(env,js,0);

    if (jpldebug)
	fprintf(stderr, "eval %s\n", (char*)jb);

    Perl_eval_pv( (char*)jb, 0 );

    if (SvTRUE(ERRSV)) {
	jthrowable newExcCls;

	(*env)->ExceptionDescribe(env);
	(*env)->ExceptionClear(env);

	newExcCls = (*env)->FindClass(env, "java/lang/RuntimeException");
	if (newExcCls)
	    (*env)->ThrowNew(env, newExcCls, SvPV(ERRSV,PL_na));
    }

    (*env)->ReleaseStringUTFChars(env,js,jb);
    FREETMPS;
    LEAVE;

}

/*
JNIEXPORT jint JNICALL
Java_PerlInterpreter_eval(JNIEnv *env, jobject obj, jint ji)
{
    op = (OP*)(void*)ji;
    op = (*op->op_ppaddr)();
    return (jint)(void*)op;
}
*/

/* Register any extra external extensions */

/* Do not delete this line--writemain depends on it */
EXTERN_C void boot_DynaLoader _((CV* cv));
EXTERN_C void boot_JNI _((CV* cv));

static void
xs_init()
{
    char *file = __FILE__;
    dXSUB_SYS;
        newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
