#ifndef CC_GFXGLES3_GLES3_FRAMEBUFFER_H_
#define CC_GFXGLES3_GLES3_FRAMEBUFFER_H_

CC_NAMESPACE_BEGIN

class GLES3GPUFramebuffer;

class CC_GLES3_API GLES3Framebuffer : public GFXFramebuffer {
 public:
  GLES3Framebuffer(GFXDevice* device);
  ~GLES3Framebuffer();
  
 public:
  bool Initialize(const GFXFramebufferInfo& info);
  void Destroy();
  
  CC_INLINE GLES3GPUFramebuffer* gpu_fbo() const { return gpu_fbo_; }
  
 private:
  GLES3GPUFramebuffer* gpu_fbo_;
};

CC_NAMESPACE_END

#endif
