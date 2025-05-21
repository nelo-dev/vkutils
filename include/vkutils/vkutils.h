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

/**
 * @file vkutils.h
 * @brief Vulkan abstraction library for C.
 * @author Nelo
 * @version 1.0
 */

#ifndef VK_UTILS_H
#define VK_UTILS_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <stdbool.h>
#include <pthread.h>
#include <cglm/cglm.h>
#include <cglm/affine.h>

#ifdef __cplusplus
    #include <atomic>
    using vku_atomic_bool = std::atomic_bool;

    #define vku_atomic_load(object) (object.load())
    #define vku_atomic_store(object, value) (object.store(value))
#else
    #include <stdatomic.h>
    typedef _Atomic(_Bool) vku_atomic_bool;

    #define vku_atomic_load(object) atomic_load(&(object))
    #define vku_atomic_store(object, value) atomic_store(&(object), value)
#endif

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

typedef struct VkuWindowCreateInfo
{
    int32_t width, height;
    bool centered;
    const char *title;
    const char *window_icon_path;
} VkuWindowCreateInfo;

typedef struct VkuWindow_t
{
    SDL_Window * sdlWindow;
    bool window_resized;
    bool minimized;
    bool fullscreen;
    int windowedX, windowedY;
    int windowedWidth, windowedHeight;
} VkuWindow_T;

typedef VkuWindow_T *VkuWindow;

/**
 * @brief Creates a window. 
 * 
 * The VkuWindow struct contains a SDL3 window handle alongside other constants for internal handling,
 * such as window resized variable signaling recreation of vulkan resources.
 * 
 * WARNING: While creating a VkuPresenter a window is created automatically.
 * @param createInfo Ptr to a VkuWindowCreateInfo struct.
 * @return A Vku Window.
 */

VkuWindow vkuCreateWindow(VkuWindowCreateInfo *createInfo);

/**
 * @brief Destroys a window.
 * 
 * WARNING: Presenter destruction also clears up window resources.
 * @param window A VkuWindow
 */

void vkuDestroyWindow(VkuWindow window);

/**
 * @brief Toggles fullscreen mode of a window
 * 
 * @param window VkuWindow.
 */

void vkuWindowToggleFullscreen(VkuWindow window);

/**
 * @brief Get the aspect-ratio of a window.
 * Returns return width / height as float value.
 * 
 * @param window VkuWindow.
 */

float vkuWindowGetAspect(VkuWindow window);

typedef struct VkuThreadSafeQueue_T {
    void **data;
    size_t capacity;
    size_t front;
    size_t rear;
    size_t size;
    pthread_mutex_t lock;
} VkuThreadSafeQueue_T;

typedef VkuThreadSafeQueue_T * VkuThreadSafeQueue;

/**
 * @brief A thread-safe FifoQueue
 * Originally an internal data struct for enqueueing buffers for destruction. May be useful.
 * 
 * @param initial_capacity The initial queue capacity. To increase performance (avoid realloc) set this value to proper value.
 * @return A VkuThreadSafeQueue handle.
 */

VkuThreadSafeQueue vkuQueueCreate(size_t initial_capacity);

/**
 * @brief Destroy a VkuThreadSafeQueue
 * 
 * @param queue A VkuThreadSafeQueue.
 */

void vkuQueueDestroy(VkuThreadSafeQueue queue);

/**
 * @brief Function to enqueue a PTR to an item to the queue.
 * 
 * WARNING: This function only enqueues an 8byte PTR value!
 * @param queue VkuThreadSafeQueue. 
 * @param item PTR to an item. 
 * @return Returns 0 on success and -1 on failure.
 */

int vkuQueueEnqueue(VkuThreadSafeQueue queue, void *item);

/**
 * @brief Dequeue an item from a VkuThreadSafeQueue
 * 
 * @param queue A VkuThreadSafeQueue. 
 * @return returns a PTR to the next item based on FIFO scheme.
 */

void *vkuQueueDequeue(VkuThreadSafeQueue queue);

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
    VkuThreadSafeQueue destructionQueue;

    VkFence *fences;
    uint32_t fenceCount;
} VkuMemoryManager_T;

typedef VkuMemoryManager_T *VkuMemoryManager;

/**
 * @brief Creates a MemoryManager
 * 
 * WARNING: A MemoryManager is already created upon VkuContext creation. You should retrieve the manager from context.
 * 
 * A VkuMemoryManager manages vulkan resources and can be used to create / destroy buffers. 
 * It handles syncronization and supports dedicated transfer queues.
 * 
 * @param createInfo PTR to VkuMemoryManagerCreateInfo struct.
 * @return A VkuMemoryManager.
 */

VkuMemoryManager vkuCreateMemoryManager(VkuMemoryManagerCreateInfo *createInfo);

/**
 * @brief Destroys VkuMemoryManager
 * 
 * @param memoryManager VkuMemoryManager
 */

void vkuDestroyMemoryManager(VkuMemoryManager memoryManager);

/**
 * @brief Get complete allocated size of buffers, etc.
 * 
 * @param manager A VkuMemoryManager.
 * @return Number of bytes allocated.
 */

VkDeviceSize vkuMemoryMamgerGetAllocatedMemorySize(VkuMemoryManager manager);

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

    vku_atomic_bool queuedForDestruction;
} VkuBuffer_T;

typedef VkuBuffer_T *VkuBuffer;

VkuBuffer vkuCreateVertexBuffer(VkuMemoryManager manager, VkDeviceSize size, VkuBufferUsage usage);
void vkuDestroyVertexBuffer(VkuBuffer buffer, VkuMemoryManager manager, VkBool32 syncronize); // Use vkuEnqueueBufferDestruction() for async buffer destruction.
void vkuSetVertexBufferData(VkuMemoryManager manager, VkuBuffer buffer, void *data, size_t size);
void vkuCopyBuffer(VkuMemoryManager manager, VkuBuffer *srcBuffer, VkuBuffer *dstBuffer, VkDeviceSize *size, uint32_t count);
void * vkuMapVertexBuffer(VkuMemoryManager manager, VkuBuffer buffer);
void vkuUnmapVertexBuffer(VkuMemoryManager manager, VkuBuffer buffer);
void vkuEnqueueBufferDestruction(VkuMemoryManager manager, VkuBuffer buffer);

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
VkuWindow vkuPresenterGetWindow(VkuPresenter presenter);
void vkuDestroyBuffersInDestructionQueue(VkuMemoryManager manager, VkuPresenter syncPresenter);
uint8_t * vkuPresenterRetrieveSwapchainImage(VkuPresenter presenter, uint32_t * outWidth, uint32_t * outHeight);
bool vkuWriteImage(const uint8_t *pixelData, uint32_t width, uint32_t height, uint32_t channels, const char *filename, bool forceOpaque);

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
    int staticDepthArrayCount;
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
void vkuRenderStageSetMSAA(VkuRenderStage renderStage, VkSampleCountFlagBits msaaFlags);

typedef struct VkuStaticRenderStageCreateInfo {
    VkuContext context;
    uint32_t width, height;
    int options;
    VkBool32 enableDepthTesting;
    VkSampleCountFlagBits msaaSamples;
    int depthLayers;
} VkuStaticRenderStageCreateInfo;

VkuRenderStage vkuCreateStaticRenderStage(VkuStaticRenderStageCreateInfo * createInfo);
void vkuDestroyStaticRenderStage(VkuRenderStage renderStage);

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
void vkuFrameDrawVertexBuffer(VkuFrame frame, VkuBuffer buffer, uint64_t vertexCount, uint32_t instanceCount);
void vkuFrameDrawVoid(VkuFrame frame, uint64_t vertexCount);

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

typedef struct VkuTextureSamplerCreateInfo
{
    VkFilter minFilter;
    VkFilter magFilter;
    VkSamplerAddressMode repeatMode;
    uint32_t mipmapLevels;
    VkBool32 enableCompare;
    VkCompareOp compareOp;
    VkBorderColor borderColor;
} VkuTextureSamplerCreateInfo;

typedef struct VkuTextureSampler_T
{
    VkSampler sampler;
} VkuTextureSampler_T;

typedef VkuTextureSampler_T *VkuTextureSampler;

VkuTextureSampler vkuCreateTextureSampler(VkuContext context, VkuTextureSamplerCreateInfo *createInfo);
void vkuDestroyTextureSampler(VkuContext context, VkuTextureSampler sampler);

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
    VkShaderStageFlagBits shaderStage;
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
    VkBool32 enableDepthBias;
    float depthBiasConstantFactor;
    float depthBiasSlopeFactor;
    VkPrimitiveTopology topology;
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