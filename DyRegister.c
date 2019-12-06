//
// Created by dragon on 2019/12/5.
//
#include "jni.h"
#include <stdlib.h>
#include <string.h>

jstring dyHelloC(JNIEnv *env, jobject obj){
    return (*env)->NewStringUTF(env, "你好,动态注册！\n哈哈哈");
}

jstring dyHello(JNIEnv *env, jobject obj, jstring jstring1){
    char ch[1024] = {0};
    jboolean  jboolean1;
    const char *string = (*env)->GetStringUTFChars(env, jstring1, &jboolean1);
    printf("isCopy:%d\n", jboolean1);
    printf("string: %s \n", string);
    if(string == NULL) {
        return NULL;
    }
    sprintf(ch,"你好,动态注册！哈哈哈 %s", string);
    printf("ch: %s \n", ch);
    (*env)->ReleaseStringUTFChars(env, jstring1, string);
    return (*env)->NewStringUTF(env, ch);
}

//需要动态注册的方法数组
static const JNINativeMethod mMethods[] = {
    {"dyHello", "()Ljava/lang/String;", (jstring *)dyHelloC},
    {"dyHello", "(Ljava/lang/String;)Ljava/lang/String;", (jstring *)dyHello}
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
    r = (*env)->RegisterNatives(env, mainActivityCls, mMethods, 2);//最后一个参数是mMethods数组的大小
    if(r != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}
