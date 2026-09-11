package com.example.openal_soft_ndk

import android.app.NativeActivity

class MainActivity : NativeActivity() {
    companion object {
        init {
            System.loadLibrary("openal")
            System.loadLibrary("openal_soft_ndk")
        }
    }
}