//
// Created by dragon on 2019/12/5.
//
#include "jni.h"

jstring dyHelloC(JNIEnv *env, jobject obj){
    return (*env)->NewStringUTF(env, "你好,动态注册！\n哈哈哈");
}

//需要动态注册的方法数组
static const JNINativeMethod mMethods[] = {
    {"dyHello", "()Ljava/lang/String;", (jstring *)dyHelloC}
};
//需要动态注册native方法的类名
static const char* mClassName = "com/ysl/myjni/Register";

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved){
    JNIEnv* env = NULL; //获得 JniEnv  
    int r = (*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4);
    if( r != JNI_OK){
    return -1;
    }
    jclass mainActivityCls = (*env)->FindClass(env, mClassName);
    // 注册 如果小于0则注册失败  
    r = (*env)->RegisterNatives(env, mainActivityCls, mMethods, 1);//最后一个参数是mMethods数组的大小
    if(r != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}
