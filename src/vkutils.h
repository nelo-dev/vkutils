/*****************************************************************************
 *
 * Filename:    vkutils.h
 * Description: Header for VkUtils C library.
 * Author:      Nelo
 * Created:     2024-11-26
 *
 * License:     MIT LICENSE
 *
 * License Terms:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *****************************************************************************/

#ifndef VK_UTILS_H
#define VK_UTILS_H

#include "../lib/vk_mem_alloc/vk_mem_alloc.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <cglm/cglm.h>
#include <cglm/affine.h>

#define VKU_VALIDATION_LAYER_NAME "VK_LAYER_KHRONOS_validation"
#define VKU_ENGINE_NAME "VKUTILS"
#define VKU_ENGINE_VERSION VK_MAKE_VERSION(1, 0, 0)

#define VK_CHECK(result)                                                                \
    do                                                                                  \
    {                                                                                   \
        VkResult res = (result);                                                        \
        if (res != VK_SUCCESS)                                                          \
        {                                                                               \
            fprintf(stderr, "VkResult failed at %s:%d: %d\n", __FILE__, __LINE__, res); \
            exit(EXIT_FAILURE);                                                         \
        }                                                                               \
    } while (0)

#define EXIT(error_message)                    \
    do                                         \
    {                                          \
        const char *message = (error_message); \
        fprintf(stderr, message);              \
        exit(EXIT_FAILURE);                    \
    } while (0)

/**
 * @brief Create a VkuWindow. Is automatically created with a presenter.
 * A VkuWindow contains a glfwWindow and other parameters to keep track of window size, aspect, fullscreen, etc.
 */

typedef struct VkuWindowCreateInfo
{
    int32_t width, height;
    bool centered;
    const char *title;
    const char *window_icon_path;
} VkuWindowCreateInfo;

typedef struct VkuWindow_t
{
    GLFWwindow *glfwWindow;
    bool window_resized;
    bool fullscreen;
    int windowedX, windowedY;
    int windowedWidth, windowedHeight;
} VkuWindow_T;

typedef VkuWindow_T *VkuWindow;

VkuWindow vkuCreateWindow(VkuWindowCreateInfo *createInfo);
void vkuDestroyWindow(VkuWindow window);
void vkuWindowToggleFullscreen(VkuWindow window);
bool vkuWindowShouldClose(VkuWindow window);
float vkuWindowGetAspect(VkuWindow window);

/**
 * @return Creates a MemoryManager. It is part of the Context but can be created seperately.
 * Memory simplifies Memory management and is used in context and presenter functions internally.
 */

typedef struct VkuMemoryManagerCreateInfo
{
    VkDevice device;
    VkQueue transferQueue;
    VkPhysicalDevice physicalDevice;
    VkInstance instance;

} VkuMemoryManagerCreateInfo;

typedef struct VkuMemoryManager_T
{
    VmaAllocator allocator;
    VkDevice device;
    VkCommandPool transferCmdPool;
    VkQueue transferQueue;

    VkFence *fences;
    uint32_t fenceCount;
} VkuMemoryManager_T;

typedef VkuMemoryManager_T *VkuMemoryManager;

VkuMemoryManager vkuCreateMemoryManager(VkuMemoryManagerCreateInfo *createInfo);
void vkuDestroyMemoryManager(VkuMemoryManager memoryManager);
VkDeviceSize vkuMemoryMamgerGetAllocatedMemorySize(VkuMemoryManager manager);

/**
 * @return a vkuBuffer
 */

typedef enum VkuBufferUsage
{
    VKU_BUFFER_USAGE_CPU_TO_GPU,
    VKU_BUFFER_USAGE_GPU_ONLY
} VkuBufferUsage;

typedef struct VkuBuffer_T
{
    VmaAllocation allocation;
    VkBuffer buffer;
    VkDeviceSize size;
} VkuBuffer_T;

typedef VkuBuffer_T *VkuBuffer;

VkuBuffer vkuCreateVertexBuffer(VkuMemoryManager manager, VkDeviceSize size, VkuBufferUsage usage);
void vkuDestroyVertexBuffer(VkuBuffer buffer, VkuMemoryManager manager);
void vkuSetVertexBufferData(VkuMemoryManager manager, VkuBuffer buffer, void *data, size_t size);
void vkuCopyBuffer(VkuMemoryManager manager, VkuBuffer *srcBuffer, VkuBuffer *dstBuffer, VkDeviceSize *size, uint32_t count);
void * vkuMapVertexBuffer(VkuMemoryManager manager, VkuBuffer buffer);
void vkuUnmapVertexBuffer(VkuMemoryManager manager, VkuBuffer buffer);

/**
 * @brief Creates Vku Context
 * Creates the basic Vulkan Ultities Context, holds basic Vulkan objects like VkInstance, VkDevice, etc.
 * @return Returns a VkuContext that can be used for further object creation, etc.
 */

typedef enum vkuContextUsageFlags
{
    VKU_CONTEXT_USAGE_BASIC = (1 << 0),
    VKU_CONTEXT_USAGE_PRESENTATION = (1 << 1),
    VKU_CONTEXT_USAGE_COMPUTE = (1 << 2)
} vkuContextUsageFlags;

typedef struct VkuContextCreateInfo
{
    VkBool32 enableValidation;
    const char *applicationName;
    uint32_t applicationVersion;
    vkuContextUsageFlags usage;
} VkuContextCreateInfo;

typedef struct VkuContext_T
{
    VkBool32 validation;
    vkuContextUsageFlags usageFlags;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;

    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;

    VkCommandPool graphicsCmdPool;
    VkuMemoryManager memoryManager;
} VkuContext_T;

typedef VkuContext_T *VkuContext;

VkuContext vkuCreateContext(VkuContextCreateInfo *createInfo);
void vkuDestroyContext(VkuContext context);
VkSampleCountFlagBits vkuContextGetMaxSampleCount(VkuContext context);
VkuMemoryManager vkuContextGetMemoryManager(VkuContext context);

/**
 * @returns a VkuPresenter which handles presentation.
 * ATTENTION! Has to be created right after Context!
 */

typedef struct VkuPresenter_T *VkuPresenter;
typedef struct VkuColorResource_T *VkuColorResource;
typedef struct VkuDepthResource_T *VkuDepthResource;

typedef struct VkuRenderResourceManager_T
{
    VkuPresenter presenter;
    uint32_t maxSamplesUint32;
    VkuColorResource *colorResources;
    VkuDepthResource *depthResources;
    VkSampleCountFlagBits *sampleFlags;
} VkuRenderResourceManager_T;

typedef VkuRenderResourceManager_T *VkuRenderResourceManager;

typedef struct VkuObjectManager_T *VkuObjectManager;

typedef struct VkuPresenterCreateInfo
{
    VkuContext context;
    int32_t width, height;
    const char *windowTitle;
    const char *windowIconPath;
    VkPresentModeKHR presentMode;
    uint32_t framesInFlight;
} VkuPresenterCreateInfo;

typedef struct VkuPresenter_T
{
    uint32_t framesInFlight;
    uint32_t currentFrame;
    VkPresentModeKHR presentMode;
    VkuContext context;

    VkuWindow window;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkImage *swapchainImages;
    VkExtent2D swapchainExtend;
    VkFormat swapchainFormat;
    uint32_t swapchainImageCount;
    VkImageView *swapchainImageViews;
    VkCommandBuffer *cmdBuffer;
    VkSemaphore *renderFinishedSemaphores;
    VkSemaphore *imageAvailableSemaphores;
    VkFence *inFlightFences;

    VkuRenderResourceManager resourceManager;
    VkuObjectManager renderStageManager;
    VkBool32 activeFrame;
} VkuPresenter_T;

typedef struct VkuPresenter_T *VkuPresenter;

VkuPresenter vkuCreatePresenter(VkuPresenterCreateInfo *createInfo);
void vkuDestroyPresenter(VkuPresenter presenter);
uint32_t vkuPresenterGetFramesInFlight(VkuPresenter presenter);
void vkuPresenterSetPresentMode(VkuPresenter presenter, VkPresentModeKHR presentMode);

/**
 * A RenderStage is one step in the Frame. It includes RenderPass Framebuffer, renderTargets.
 * It can render to the swapchain or to internal Images that can be retrieved as Texture.
 */

typedef enum VkuRenderOptions
{
    VKU_RENDER_OPTION_COLOR_IMAGE = (1 << 0),
    VKU_RENDER_OPTION_DEPTH_IMAGE = (1 << 1),
    VKU_RENDER_OPTION_PRESENTER = (1 << 2)
} VkuRenderOptions;

typedef struct VkuRenderStageCreateInfo
{
    VkSampleCountFlagBits msaaSamples;
    VkuPresenter presenter;
    int options;
    VkBool32 enableDepthTesting;
} VkuRenderStageCreateInfo;

typedef struct VkuRenderStage_T
{
    VkuPresenter presenter;
    VkuContext context;
    VkSampleCountFlagBits sampleCount;
    int options;
    VkBool32 enableDepthTesting;
    VkBool32 outputCount;

    VkBool32 staticRenderStage;
    VkExtent2D extend;

    VkRenderPass renderPass;
    VkFramebuffer *framebuffers;
    VkuColorResource colorResource;
    VkuDepthResource depthResource;
    VkImage *pTargetColorImages;
    VkImageView *pTargetColorImgViews;
    VmaAllocation *pTargetColorImgAllocs;
    VkImage *pTargetDepthImages;
    VkImageView *pTargetDepthImgViews;
    VmaAllocation *pTargetDepthImgAllocs;

    VkuObjectManager pipelineManager;
    VkuObjectManager outputTextureManager;
    VkuObjectManager descriptorSetManager;
} VkuRenderStage_T;

typedef VkuRenderStage_T *VkuRenderStage;

VkuRenderStage vkuCreateRenderStage(VkuRenderStageCreateInfo *createInfo);
void vkuDestroyRenderStage(VkuRenderStage renderStage);

/**
 * A VkuStaticRenderStage is independent of a VkuPresenter and has a fixed resolution.
 * That means nothing gets recreated when window resize & you cant render into Swapchain (only texture output)
 */

typedef struct VkuStaticRenderStageCreateInfo {
    VkuContext context;
    uint32_t width, height;
    int options;
    VkBool32 enableDepthTesting;
    VkSampleCountFlagBits msaaSamples;
} VkuStaticRenderStageCreateInfo;

VkuRenderStage vkuCreateStaticRenderStage(VkuStaticRenderStageCreateInfo * createInfo);
void vkuDestroyStaticRenderStage(VkuRenderStage renderStage);

/**
 * Presenter functions for render tasks in a frame.
 */

typedef struct VkuFrame_T
{
    VkuPresenter presenter;
    uint32_t imageIndex;
    VkCommandBuffer cmdBuffer;

    bool activeRenderStage;
} VkuFrame_T;

typedef struct VkuFrame_T *VkuFrame;

VkuFrame vkuPresenterBeginFrame(VkuPresenter presenter);
void vkuPresenterSubmitFrame(VkuFrame frame);
void vkuFrameBeginRenderStage(VkuFrame frame, VkuRenderStage renderStage);
void vkuFrameFinishRenderStage(VkuFrame frame, VkuRenderStage renderStage);
void vkuFrameDrawVertexBuffer(VkuFrame frame, VkuBuffer buffer, uint64_t vertexCount);
void vkuFrameDrawVoid(VkuFrame frame, uint64_t vertexCount);
/**
 * Textures
 */

typedef struct VkuTexture2DCreateInfo
{
    int width;
    int height;
    int channels;
    uint8_t *pixelData;
    int mipLevels;
} VkuTexture2DCreateInfo;

typedef struct VkuTexture2D_T
{
    VkImage textureImage;
    VmaAllocation textureImageAllocation;
    VkImageView textureImageView;
    VkExtent2D imageExtend;

    VkBool32 renderStageColorImage;
    VkBool32 renderStageDepthImage;
    VkuRenderStage renderStage;
} VkuTexture2D_T;

typedef VkuTexture2D_T *VkuTexture2D;

typedef struct VkuTexture2DArrayCreateInfo
{
    int width;
    int height;
    int channels;
    int layerCount;
    int mipLevels;
    uint8_t **pixelDataArray;
} VkuTexture2DArrayCreateInfo;

typedef struct VkuTexture2DArray_T
{
    VkImage textureImage;
    VmaAllocation textureImageAllocation;
    VkImageView textureImageView;
} VkuTexture2DArray_T;

typedef VkuTexture2DArray_T *VkuTexture2DArray;

uint8_t *vkuLoadImage(const char *path, int *width, int *height, int *channels);
VkuTexture2DArray vkuCreateTexture2DArray(VkuContext context, VkuTexture2DArrayCreateInfo *createInfo);
void vkuDestroyTexture2DArray(VkuContext context, VkuTexture2DArray texArray);
VkuTexture2D vkuCreateTexture2D(VkuContext context, VkuTexture2DCreateInfo *createInfo);
void vkuDestroyTexture2D(VkuContext context, VkuTexture2D texture);
VkuTexture2D vkuRenderStageGetDepthOutput(VkuRenderStage renderStage);
VkuTexture2D vkuRenderStageGetColorOutput(VkuRenderStage renderStage);

// VkuSampler

typedef struct VkuTextureSamplerCreateInfo
{
    VkFilter minFilter;
    VkFilter magFilter;
    VkSamplerAddressMode repeatMode;
    uint32_t mipmapLevels;
} VkuTextureSamplerCreateInfo;

typedef struct VkuTextureSampler_T
{
    VkSampler sampler;
} VkuTextureSampler_T;

typedef VkuTextureSampler_T *VkuTextureSampler;

VkuTextureSampler vkuCreateTextureSampler(VkuContext context, VkuTextureSamplerCreateInfo *createInfo);
void vkuDestroyTextureSampler(VkuContext context, VkuTextureSampler sampler);

/**
 * @return VkuUniform Buffer. The bufferCount have to match the FramesInFlight of the Presenter!
 */

typedef struct VkuUniformBuffer_T
{
    void **mappedMemory;
    VmaAllocation *uniformAllocs;
    VkBuffer *uniformBuffer;
    uint32_t bufferCount;
    VkDeviceSize bufferSize;
} VkuUniformBuffer_T;

typedef VkuUniformBuffer_T *VkuUniformBuffer;

VkuUniformBuffer vkuCreateUniformBuffer(VkuContext context, VkDeviceSize bufferSize, uint32_t count);
void vkuDestroyUniformBuffer(VkuContext context, VkuUniformBuffer uniformBuffer);
void vkuFrameUpdateUniformBuffer(VkuFrame frame, VkuUniformBuffer uniBuffer, void *data);

/**
 * DescriptorSet
 */

typedef enum descriptorAttributeOptions
{
    VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER,
    VKU_DESCRIPTOR_SET_ATTRIB_UNIFORM_BUFFER
} descriptorAttributeOptions;

typedef struct VkuDescriptorSetAttribute
{
    descriptorAttributeOptions type;
    VkuTextureSampler sampler;
    VkuTexture2D tex2D;
    VkuTexture2DArray tex2DArray;
    VkuUniformBuffer uniformBuffer;
} VkuDescriptorSetAttribute;

typedef struct VkuDescriptorSetCreateInfo
{
    VkuDescriptorSetAttribute *attributes;
    uint32_t attributeCount;
    VkuRenderStage renderStage;
    uint32_t descriptorCount;
} VkuDescriptorSetCreateInfo;

typedef struct VkuDescriptorSet_T
{
    VkuRenderStage renderStage;
    VkuDescriptorSetAttribute *attributes;
    uint32_t attributeCount;
    uint32_t setCount;

    VkDescriptorSetLayout setLayout;
    VkDescriptorPool pool;
    VkDescriptorSet *sets;
} VkuDescriptorSet_T;

typedef VkuDescriptorSet_T *VkuDescriptorSet;

VkuDescriptorSet vkuCreateDescriptorSet(VkuDescriptorSetCreateInfo *createInfo);
void vkuDestroyDescriptorSet(VkuDescriptorSet set);

/**
 * Pipeline
 */

typedef struct VkuVertexAttribute
{
    VkFormat format;
    uint32_t offset;
} VkuVertexAttribute;

typedef struct VkuVertexLayout
{
    uint32_t attributeCount;
    VkuVertexAttribute *attributes;
    uint32_t vertexSize;
} VkuVertexLayout;

typedef struct VkuPipelineCreateInfo
{
    char *vertexShaderSpirV;
    uint32_t vertexShaderLength;
    char *fragmentShaderSpirV;
    uint32_t fragmentShaderLength;
    VkuVertexLayout vertexLayout;
    VkPolygonMode polygonMode;
    VkuDescriptorSet descriptorSet;
    VkBool32 depthTestWrite;
    VkBool32 depthTestEnable;
    VkCompareOp depthCompareMode;
    VkCullModeFlags cullMode;
    VkuRenderStage renderStage;
} VkuPipelineCreateInfo;

typedef struct VkuPipeline_T
{
    VkuPipelineCreateInfo recreateInfo;
    VkuVertexAttribute *vertexAttributes;
    VkuVertexLayout vertexLayout;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkuDescriptorSet descriptorSet;
    VkuRenderStage renderStage;

    char *internalVertexSpirv;
    char *internalFragmentSpirv;
} VkuPipeline_T;

typedef VkuPipeline_T *VkuPipeline;

char *vkuReadFile(const char *filename, uint32_t *length);
VkuPipeline vkuCreatePipeline(VkuContext context, VkuPipelineCreateInfo *createInfo);
void vkuDestroyPipeline(VkuContext context, VkuPipeline pipeline);
void vkuFrameBindPipeline(VkuFrame frame, VkuPipeline pipeline);
void vkuFramePipelinePushConstant(VkuFrame frame, VkuPipeline pipeline, void *data, size_t size);

#endif