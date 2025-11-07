#pragma once

// 纹理单元宏（27-31，集中管理）
#define TEX_SLOT_SCENE        28  // HDR场景
#define TEX_SLOT_BLOOM_BLUR   27  // 泛光模糊
#define TEX_SLOT_SHADOW_PARA  29  // 平行光阴影
#define TEX_SLOT_SHADOW_CUBE  30  // 点光源阴影
#define TEX_SLOT_QUAD         31  // 全屏四边形
#define TEX_SLOT_BLOOM_SHADER 26  // blurshader

// 分辨率配置（保留动态性）
struct RendererConfig {
    int windowWidth = 0;
    int windowHeight = 0;
    struct {
        int shadowMap = 1024;
    } textureResolutions;

    struct GlobalRenderSettings {
        float exposure = 1.0f;
        bool  enableBloom = true;
        bool  enableHDR = true;
        bool  enableGamma = true;
    } renderSettings;
};