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

static jobject ref;//全局引用
static jclass weakRef;//弱全局引用
void getObj(JNIEnv *env, jobject obj){
    jclass pclz = (*env)->FindClass(env, "com/ysl/myjni/Method");//得到的是局部引用变量；
    if (pclz == NULL){
        printf("jclass is null");
        return;
    }
    //调用构造函数
    //得到的不是局部引用；而是JNIInvalidRefType,位于jni.h中的typedef enum _jobjectType
    jmethodID pId = (*env)->GetMethodID(env, pclz, "<init>", "()V");//得到的不是局部引用；
    if (pId == NULL){
        printf("jmethodID is null");
        return;
    }
    //局部引用
    jobject pJobject = (*env)->NewObject(env, pclz, pId);//这里返回的就是Java类的实例对象

    //获取getName方法的id  非静态方法
    jmethodID pJmethodId = (*env)->GetMethodID(env, pclz, "getName", "(Ljava/lang/String;)V");
    jstring utf = (*env)->NewStringUTF(env, "--->hello  ");//局部引用
    //第二个参数是得到的object对象，方法id，参数
    (*env)->CallVoidMethod(env, pJobject, pJmethodId, utf);

    //获取getMsg方法的id  静态方法
    jmethodID staticmethodId = (*env)->GetStaticMethodID(env, pclz, "getMsg", "(Ljava/lang/String;I)V");
    jstring name = (*env)->NewStringUTF(env, "ysl");//局部引用
    jint age = 30;
    //第二个参数是class对象，方法id，参数
    (*env)->CallStaticVoidMethod(env, pclz, staticmethodId, name, age);

    //全局引用，阻止GC回收，可跨方法，跨线程使用；。JVM不会自动释放， 必须调用DeleteGlobalRef
    // 手动释放 (*env)->DeleteGlobalRef(env,g_cls_string);
    ref = (*env)->NewGlobalRef(env, pJobject);
    //在合适的时候调用DeleteGlobalRef释放内存，被GC回收
    //(*env)->DeleteGlobalRef(env, pJobject);

    //调用NewWeakGlobalRef基于局部引用或全局引用创建，不会阻止GC回收所引用的对象，可以跨方法、
    // 跨线程使 用。引用不会自动释放，在JVM认为应该回收它的时候（比如内存紧张的时候）进行回收而被释放。
    // 或调用 DeleteWeakGlobalRef手动释放。(*env)->DeleteWeakGlobalRef(env,g_cls_string)
    weakRef = (*env)->NewWeakGlobalRef(env, pclz);
    //(*env)->DeleteWeakGlobalRef(env,pclz);

    //通过NewLocalRef和各种JNI接口创建（FindClass、NewObject、GetObjectClass和NewCharArray等）。
    // 会阻止GC 回收所引用的对象，不能本地函数中跨函数使用，不能跨线前使用。函数返回后局部引用所
    // 引用的对象会被JVM自动 释放，或调用DeleteLocalRef释放。 (*env)->DeleteLocalRef(env,local_ref)
    (*env)->DeleteLocalRef(env, pJobject);
    (*env)->DeleteLocalRef(env, utf);
    (*env)->DeleteLocalRef(env, pclz);
    (*env)->DeleteLocalRef(env, name);
    // 1）绝大部分JNI方法返回的是局部引用；
    // 2）局部引用的作用域或生命周期始于创建它的本地方法，终于本地方法返回
    // 3）通常在局部引用不再使用时，可以显示使用DeleteLocalRef来提前释放它所指向的对象，以便于GC回收。
    // 4） 局部引用是线程相关的，只能在创建它的线程里使用，通过全局变量缓存并使用在其他线程是不合法的。

    //全局引用必须手动释放内存
    (*env)->DeleteGlobalRef(env, pJobject);
    //在JVM认为应该回收它的时候（比如内存紧张的时候）进行回收而被释放。
    // 或调用 DeleteWeakGlobalRef手动释放
    (*env)->DeleteWeakGlobalRef(env, pclz);
}


jstring newString(JNIEnv * env, jobject jobj, jint len){
    jcharArray elemArray;
    jchar *chars = NULL;
    jstring j_str = NULL;
    // 定义静态的局部变量,定义后地址不会释放；但由于是局部引用，第一次方法结束后，
    // 地址内存储的值已被回收，所以可能出现野指针的bug
    static jclass cls_string = NULL;
    static jmethodID  jmetid = NULL;

    if (cls_string == NULL) {
        printf("cls_string is null \n");
        cls_string = (*env)->FindClass(env, "java/lang/String");
        if (cls_string == NULL) {
            return NULL;
        }
    }

    if(jmetid == NULL) {
        printf("jmetid is null \n");
        jmetid = (*env)->GetMethodID(env, cls_string, "<init>", "([C)V");
        if (jmetid == NULL) {
            return NULL;
        }
    }
    printf("this is a line -------------\n");

    elemArray = (*env)->NewCharArray(env, len);
    printf("this is a line2 -------------\n");
    printf("this is a %d,%d,%d\n", cls_string, jmetid, elemArray);

    //如果不释放上面静态局部变量，第二次这里会报错
    j_str = (*env)->NewObject(env, cls_string, jmetid, elemArray);
    printf("this is a line3 -------------\n");
    (*env)->DeleteLocalRef(env, elemArray);
    const char * ch = (*env)->GetStringUTFChars(env,j_str,NULL);

    elemArray = NULL;

    //把两个静态局部变量回收，否则会报野指针错误
    (*env)->DeleteLocalRef(env, cls_string);//1.先回收
    cls_string = NULL;//2.这里必须赋值为null，否则指针还存在
    //    // 此处的 delete不能存在，因为 jmetid不是jobject，应用只需要对object类型的引用而言的，
    //     (*env)->DeleteLocalRef(env, jmetid);
    jmetid = NULL;//赋值为null

    printf("end of function \n");
    printf("\n");
    return j_str;
}
jstring javaCall(JNIEnv * env, jobject jobj){
    return newString(env, jobj, 10);
}

//需要动态注册的方法数组
static const JNINativeMethod mMethods[] = {
    {"dyHello", "()Ljava/lang/String;", (jstring *)dyHelloC},
    {"dyHello", "(Ljava/lang/String;)Ljava/lang/String;", (jstring *)dyHello},
    {"getField", "()V", getField},
    {"getStaField", "()V", getStaField},
    {"getMethod", "()V", getObj},
    {"newString", "(I)Ljava/lang/String;", newString},
    {"javaCall", "()Ljava/lang/String;", (jstring *)javaCall},
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
    r = (*env)->RegisterNatives(env, mainActivityCls, mMethods, 7);//最后一个参数是mMethods数组的大小
    if(r != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}
