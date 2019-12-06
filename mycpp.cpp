//
// Created by dragon on 2019/12/6.
//
#include "jni.h"
//静态注册，有一个完整的类名
JNIEXPORT jstring JNICALL Java_com_ysl_myjni_Register_helloworld
        (JNIEnv * env, jobject jobject){
    return env->NewStringUTF("你好！everybody\n我很好");
}

