#pragma once
#include "../includes.h"

// Render to texture.
// We save the framebuffer to a texture
class RenderToTexture
{

protected:

    // Identifiers for the gpu
    GLuint frambuffer_name_ = 0;
    // Color buffer, texture colormap
    GLuint colorbuffer_;
    // Depth map identifier
    GLuint depthbuffer_;

public:
    RenderToTexture();
    RenderToTexture(const char* name, int xres, int yres);
    ~RenderToTexture();

    bool Init();
    void Activate();
    void Deactivate();
    void Destroy();

    GLuint GetFrameBufferName() {
        return frambuffer_name_;
    }

    GLuint GetColorBuffer() {
        return colorbuffer_;
    }

    GLuint GetDepthBuffer() {
        return depthbuffer_;
    }

private:

    int xres_ = 0;
    int yres_ = 0;
    const char* name_;
};
