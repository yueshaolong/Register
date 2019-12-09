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
    jboolean  jboolean1;//是否copy成功
    const char *string = (*env)->GetStringUTFChars(env, jstring1, &jboolean1);
    printf("isCopy:%d\n", jboolean1);
    printf("string: %s \n", string);
    //调用完GetStringUTFChars之后不要忘记安全检查，因为JVM需要为新诞生的字符串分配内存空间，
    // 当内存空间不够分配的时候，会导致调用失败，失败后GetStringUTFChars会返回NULL，并抛出一个
    // OutOfMemoryError异常。JNI的异常和Java中的异常处理流程是不一样的，Java遇到异常如果没有捕获，
    // 程序会立即停止运行。而JNI遇到未决的异常不会改变程序的运行流程，也就是程序会继续往下走，
    // 这样后面针对这个字符串的所有操作都是非危险的，因此，我们需要用return语句跳过后面的代码，
    // 并立即结束当前方法。
    if(string == NULL) {
        return NULL;
    }
//    sprintf(ch,"你好,动态注册！哈哈哈 %s", string);
    strcat(ch, string);
    strcat(ch, "你好,动态注册！哈哈哈 ");
    printf("ch: %s \n", ch);
    //在调用GetStringUTFChars函数从JVM内部获取一个字符串之后，JVM内部会分配一块新的内存，
    // 用于存储源字符串的拷贝，以便本地代码访问和修改。即然有内存分配，用完之后马上释放是
    // 一个编程的好习惯。通过调用ReleaseStringUTFChars函数通知JVM这块内存已经不使用了，
    // 你可以清除了。注意：这两个函数是配对使用的，用了GetXXX就必须调用ReleaseXXX，而且
    // 这两个函数的命名也有规律，除了前面的Get和Release之外，后面的都一 样。
    (*env)->ReleaseStringUTFChars(env, jstring1, string);
    return (*env)->NewStringUTF(env, ch);
}

void getField(JNIEnv *env, jobject obj){
    jclass aClass = (*env)->GetObjectClass(env, obj);
    jfieldID pId = (*env)->GetFieldID(env, aClass, "num", "I");
    jint i = (*env)->GetIntField(env, obj, pId);
    printf("C获取的num值为：%d\n", i);
    (*env)->SetIntField(env, obj, pId, i+100);
}

void getStaField(JNIEnv *env, jobject obj){
    jclass clz = (*env)->GetObjectClass(env, obj);
    jfieldID pId = (*env)->GetStaticFieldID(env, clz, "staNum", "I");
    jint i = (*env)->GetStaticIntField(env, clz, pId);
    printf("C获取的num值为：%d\n", i);
    (*env)->SetStaticIntField(env, obj, pId, i * 10);
}

//需要动态注册的方法数组
static const JNINativeMethod mMethods[] = {
    {"dyHello", "()Ljava/lang/String;", (jstring *)dyHelloC},
    {"dyHello", "(Ljava/lang/String;)Ljava/lang/String;", (jstring *)dyHello},
    {"getField", "()V", getField},
    {"getStaField", "()V", getStaField},
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
    r = (*env)->RegisterNatives(env, mainActivityCls, mMethods, 4);//最后一个参数是mMethods数组的大小
    if(r != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}
