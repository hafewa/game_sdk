#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <string>
#include <android/log.h>
#include "NativeHelpher.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TAG "NATIVE_READ_BYTE"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


static AAssetManager *assetManager = nullptr;


JNIEXPORT void JNICALL SetAssetManager
(JNIEnv *env, jobject jobj, jobject jassetManager) {
LOGD("set asset manager");
assetManager = AAssetManager_fromJava(env, jassetManager);
}

JNIEXPORT int32_t JNICALL ReadAssetsBytes(char* fileName, unsigned char** result){

    if(assetManager == nullptr){
        return -1;
    }
    AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_UNKNOWN);
    if(asset == nullptr){
        return -1;
    }
    off_t size = AAsset_getLength(asset);
    if(size > 0){
        *result = new unsigned char[size];
        AAsset_read(asset, *result, size);
    }
    AAsset_close(asset);

    return (int32_t)size;
}

JNIEXPORT int32_t JNICALL ReadAssetsBytesWithOffset(char* fileName, unsigned char** result, int32_t offset, int32_t length){

    if(assetManager == nullptr){
        return -1;
    }
    AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_UNKNOWN);
    if(asset == nullptr){
        return -1;
    }
    off_t size = AAsset_getLength(asset);
    if(size > 0){
        try {
            *result = new unsigned char[length];
            AAsset_seek(asset, offset, SEEK_SET);
            AAsset_read(asset, *result, length);
        }catch (std::bad_alloc){
            *result = nullptr;
            return -1;
        }
    }
    AAsset_close(asset);

    return (int32_t)length;
}

JNIEXPORT int32_t JNICALL ReadRawBytes(char* fileName, unsigned char** result){
    if(fileName == nullptr){
        return -1;
    }
    FILE* file = fopen(fileName, "r");
    if(file == nullptr){
        return -2;
    }
    fseek(file, 0L, SEEK_END);
    int32_t size = ftell(file);
    if(size <= 0){
        return -3;
    }
    *result = new uint8_t[size];
    fseek(file, 0, SEEK_SET);
    fread(*result, sizeof(uint8_t), static_cast<size_t>(size), file);
    fclose(file);
    return size;
}


JNIEXPORT void JNICALL ReleaseBytes(unsigned char* bytes){
    delete[] bytes;
}

JNIEXPORT int32_t Add(int32_t a, int32_t b)
{
    return a + b;
}

#ifdef __cplusplus
}
#endif