#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <vector>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <AL/al.h>
#include <AL/alc.h>

#define LOG_TAG "GLES_NATIVE"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))

void OpenAL_Test(void){
    ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    const int SR = 48000;
    const int FREQ = 440;
    const int SECONDS = 2;
    const size_t bufferSize = SR * SECONDS;
    std::vector<short> pcmData(bufferSize);

    for (size_t i = 0; i < bufferSize; ++i) {
        pcmData[i] = static_cast<short>(32767.0 * sin(2.0 * M_PI * FREQ * i / SR));
    }


    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, pcmData.data(), pcmData.size() * sizeof(short), SR);

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);

    alSourcePlay(source);
}

// アプリの状態を管理する構造体
struct AppState {
    android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    bool animating;
};

// EGL（OpenGLの画面接続システム）の初期化
static int init_display(AppState* state) {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };
    EGLint format;
    EGLint numConfigs;
    EGLConfig config;

    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(state->display, nullptr, nullptr);
    eglChooseConfig(state->display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(state->display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(state->app->window, 0, 0, format);

    // OpenGL ES 3.0 の文脈（コンテキスト）を作成
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    state->surface = eglCreateWindowSurface(state->display, config, state->app->window, nullptr);
    state->context = eglCreateContext(state->display, config, nullptr, contextAttribs);

    if (eglMakeCurrent(state->display, state->surface, state->surface, state->context) == EGL_FALSE) {
        LOGI("EGLコンテキストの有効化に失敗しました。");
        return -1;
    }
    OpenAL_Test();

    return 0;
}

// 毎フレームの描画（ここで画面の色を決めています）
static void draw_frame(AppState* state) {
    if (state->display == nullptr) return;
    glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    eglSwapBuffers(state->display, state->surface);
}

// 片付け処理
static void terminate_display(AppState* state) {
    if (state->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (state->context != EGL_NO_CONTEXT) eglDestroyContext(state->display, state->context);
        if (state->surface != EGL_NO_SURFACE) eglDestroySurface(state->display, state->surface);
        eglTerminate(state->display);
    }
    state->animating = false;
    state->display = EGL_NO_DISPLAY;
    state->context = EGL_NO_CONTEXT;
    state->surface = EGL_NO_SURFACE;
}

// スマホのシステムイベント（画面ができた、消えた等）の処理
static void handle_cmd(android_app* app, int32_t cmd) {
    auto* state = (AppState*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (state->app->window != nullptr) {
                init_display(state);
                state->animating = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            terminate_display(state);
            break;
        case APP_CMD_GAINED_FOCUS:
            state->animating = true;
            break;
        case APP_CMD_LOST_FOCUS:
            state->animating = false;
            draw_frame(state);
            break;
    }
}

// ネイティブアプリのメイン関数
void android_main(android_app* state) {
    AppState appState{};
    state->userData = &appState;
    state->onAppCmd = handle_cmd;
    appState.app = state;

    while (true) {
        int ident;
        int events;
        android_poll_source* source;

        // イベントの監視（待機）
        while ((ident = ALooper_pollOnce(appState.animating ? 0 : -1, nullptr, &events, (void**)&source)) >= 0) {
            if (source != nullptr) source->process(state, source);
            if (state->destroyRequested != 0) {
                terminate_display(&appState);
                return;
            }
        }

        // 画面が表示状態なら毎フレーム描画
        if (appState.animating) {
            draw_frame(&appState);
        }
    }
}