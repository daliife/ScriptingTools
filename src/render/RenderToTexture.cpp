#include "RenderToTexture.h"

// Temporal render to texture
// This should be improved and move somewhere else

RenderToTexture::RenderToTexture()
{
    name_ = "renderTexture";
    xres_ = 1024;
    yres_ = 768;
    
    Init();
}

RenderToTexture::RenderToTexture(const char* name, int new_xres, int new_yres)
{
    name_ = name;
    xres_ = new_xres;
    yres_ = new_yres;

    Init();
}

RenderToTexture::~RenderToTexture()
{
    Destroy();
}

// Initialize the opengl framebuffer.
bool RenderToTexture::Init() {

    glGenFramebuffers(1, &frambuffer_name_);
    glBindFramebuffer(GL_FRAMEBUFFER, frambuffer_name_);

    // The texture we're going to render to
    glGenTextures(1, &colorbuffer_);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, colorbuffer_);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xres_, yres_, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // The depth buffer
    glGenRenderbuffers(1, &depthbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, xres_, yres_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer_);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorbuffer_, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    return true;
}

// Activating render to texture
// Everything rendered will be saved into this texture
void RenderToTexture::Activate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, frambuffer_name_);
}

// Deactivate render to texture, so we are back to our main buffer
void RenderToTexture::Deactivate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::Destroy()
{
    // Delete the framebuffer object

}
