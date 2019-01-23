#include <jni.h>
#include <string>

#include <atomic>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include <android/log.h>
#include <unordered_set>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include <syscall.h>
#include "linker.h"
#include "hooks.h"

#define  LOG_TAG    "HOOOOOOOOK"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

std::atomic<bool> socket_hooked;

static jclass kJavaClass;
static jmethodID kMethodGetStack;
static JavaVM *kJvm;


char *jstringToChars(JNIEnv *env, jstring jstr) {
    if (jstr == nullptr) {
        return nullptr;
    }

    jboolean isCopy = JNI_FALSE;
    const char *str = env->GetStringUTFChars(jstr, &isCopy);
    char *ret = strdup(str);
    env->ReleaseStringUTFChars(jstr, str);
    return ret;
}

void printJavaStack() {
    JNIEnv* jniEnv = NULL;
    // JNIEnv 是绑定线程的，所以这里要重新取
    kJvm->GetEnv((void**)&jniEnv, JNI_VERSION_1_6);
    jstring java_stack = static_cast<jstring>(jniEnv->CallStaticObjectMethod(kJavaClass, kMethodGetStack));
    if (NULL == java_stack) {
        return;
    }
    char* stack = jstringToChars(jniEnv, java_stack);
    ALOG("stack:%s", stack);
    free(stack);

    jniEnv->DeleteLocalRef(java_stack);
}


int socket_connect_hook(int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen) {
    printJavaStack();
    ALOG("socket_connect_hook!!!!!");

    return CALL_PREV(socket_connect_hook, sockfd, serv_addr, addrlen);
}

ssize_t socket_send_hook(int sockfd, const void *buf, size_t len, int flags) {
    ALOG("socket_send_hook!!!!");

    return CALL_PREV(socket_send_hook, sockfd, buf, len, flags);
}

ssize_t socket_recv_hook(int sockfd, void *buf, size_t len, int flags) {
    ALOG("socket_recv_hook!!!!!");

    return CALL_PREV(socket_recv_hook, sockfd, buf, len, flags);
}

ssize_t socket_sendto_hook(int sockfd, const void *buf, size_t len, int flags,
                           const struct sockaddr *dest_addr, socklen_t addrlen) {
    ALOG("socket_sendto_hook!!!!!");

    return CALL_PREV(socket_sendto_hook, sockfd, buf, len, flags, dest_addr, addrlen);
}

ssize_t socket_recvfrom_hook(int sockfd, void *buf, size_t len, int flags,
                             struct sockaddr *src_addr, socklen_t *addrlen) {
    ALOG("socket_recvfrom_hook!!!!!");

    return CALL_PREV(socket_recvfrom_hook, sockfd, buf, len, flags, src_addr, addrlen);
}

/**
* plt hook libc 的 pthread_create 方法
*/
void hookLoadedLibs() {
    ALOG("hook_plt_method");
    hook_plt_method("libart.so", "send", (hook_func) &socket_send_hook);
    hook_plt_method("libart.so", "recv", (hook_func) &socket_recv_hook);
    hook_plt_method("libart.so", "sendto", (hook_func) &socket_sendto_hook);
    hook_plt_method("libart.so", "recvfrom", (hook_func) &socket_recvfrom_hook);
    hook_plt_method("libart.so", "connect", (hook_func) &socket_connect_hook);
}


void enableSocketHook() {
    if (socket_hooked) {
        return;
    }
    ALOG("enableSocketHook");

    socket_hooked = true;
    if (linker_initialize()) {
        throw std::runtime_error("Could not initialize linker library");
    }
    hookLoadedLibs();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dodola_socket_SocketHook_enableSocketHookNative(JNIEnv *env, jclass type) {

    enableSocketHook();
}

static bool InitJniEnv(JavaVM *vm) {
    kJvm = vm;
    JNIEnv* env = NULL;
    if (kJvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK){
        ALOG("InitJniEnv GetEnv !JNI_OK");
        return false;
    }
    kJavaClass = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/dodola/socket/SocketHook")));
    if (kJavaClass == NULL)  {
        ALOG("InitJniEnv kJavaClass NULL");
        return false;
    }

    kMethodGetStack = env->GetStaticMethodID(kJavaClass, "getStack", "()Ljava/lang/String;");
    if (kMethodGetStack == NULL) {
        ALOG("InitJniEnv kMethodGetStack NULL");
        return false;
    }
    return true;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    ALOG("JNI_OnLoad");


    if (!InitJniEnv(vm)) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

