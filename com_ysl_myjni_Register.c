//
// Created by dragon on 2019/12/5.
//

//<>表示系统.h库；""表示自己的.h文件
#include "com_ysl_myjni_Register.h"

//静态注册，有一个完整的类名
JNIEXPORT jstring JNICALL Java_com_ysl_myjni_Register_helloworld
(JNIEnv * env, jobject jobject){
    return (*env)->NewStringUTF(env, "你好！everybody\n我很好");
}

