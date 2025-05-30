/*****************************************************************************
 *
 * Filename:    vkutils.c
 * Description: Implementation for VkUtils C library.
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

#include <vkutils/vkutils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

#define CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

// Internal Function Predefines

typedef struct VkuObjectManager_T
{
    size_t elemSize;
    uint32_t elemCnt;
    void **elements;
} VkuObjectManager_T;

typedef struct VkuObjectManager_T *VkuObjectManager;

VkuObjectManager vkuCreateObjectManager(size_t elementSize);
void vkuObjectManagerAdd(VkuObjectManager objectManager, void *element);
void vkuObjectManagerRemove(VkuObjectManager objectManager, void *element);
int vkuDestroyObjectManager(VkuObjectManager objectManager);
bool vkuContains(uint32_t *array, uint32_t count, uint32_t value);
void vkuAddStringToArray(char ***array, uint32_t *stringCount, const char *newString);
void vkuFreeStringArray(char **array, size_t size);
char *vkuReadFile(const char *filename, uint32_t *length);
int find_bit_position(uint32_t value);

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
void vkuPopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo, VkBool32 verboseOutput);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
VkDebugUtilsMessengerEXT vkuCreateVkDebugMessenger(VkInstance instance, VkBool32 validation);
void vkuDestroyVkDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger);

typedef struct VkuVkInstanceCreateInfo
{
    VkBool32 enableValidation;
    const char *applicationName;
    uint32_t applicationVersion;
} VkuVkInstanceCreateInfo;

char **vkuGetRequiredInstanceExtensions(uint32_t *pExtensionCount);
char **vkuGetIntanceLayers(uint32_t *pLayerCount);
bool vkuCheckLayersAvailable(uint32_t layer_count, char **layers);
bool vkuCheckExtensionAvailable(uint32_t extension_count, char **extensions);
VkInstance vkuCreateVkInstance(VkuVkInstanceCreateInfo *createInfo);
void vkuDestroyVkInstance(VkInstance instance);

VkSurfaceKHR vkuCreateSurface(VkInstance instance, SDL_Window * sdlWindow);
void vkuDestroySurface(VkSurfaceKHR surface, VkInstance instance);

typedef struct VkuQueueFamilyIndices
{
    uint32_t graphicsQueueFam;
    uint32_t presentQueueFam;
    uint32_t computeQueueFam;
    uint32_t transferQueueFam;
} VkuQueueFamilyIndices;

typedef struct VkuVkDeviceCreateInfo
{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkBool32 enable_validation;
    VkQueue *pGraphicsQueue, *pPresentQueue, *pTransferQueue, *pComputeQueue;
} VkuVkDeviceCreateInfo;

char **vkuGetDeviceExtensions(uint32_t *pDeviceExtensionCount);
void vkuGetQueueFamilyIndices(VkuQueueFamilyIndices *indices, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void vkuGetUniqueQueueFamilyIndices(VkuQueueFamilyIndices *indices, uint32_t *unique_queue_family_count, uint32_t unique_queue_families[4]);
VkDevice vkuCreateVkDevice(VkuVkDeviceCreateInfo *create_info);
void vkuDestroyVkDevice(VkDevice device);

typedef struct VkuSwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats;
    uint32_t formatsCount;
    VkPresentModeKHR *presentModes;
    uint32_t presentModesCount;
} VkuSwapchainSupportDetails;

VkSampleCountFlagBits vkuGetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);
void vkuQuerySwapchainSupport(VkuSwapchainSupportDetails *details, VkPhysicalDevice physical_device, VkSurfaceKHR surface);
void vkuDestroySwapchainSupportDetails(VkuSwapchainSupportDetails *details);
bool vkuCheckPhysicalDeviceExtensionSupport(VkPhysicalDevice physical_device, char **deviceExtensions, uint32_t deviceExtensionCount);
bool vkuComparePhysicalDevices(VkPhysicalDevice device1, VkPhysicalDevice device2);
VkPhysicalDevice *vkuGetSuitableVkPhysicalDevices(uint32_t *pPhysicalDeviceCount, VkSurfaceKHR surface, VkInstance instance);
void vkuPrintVkPhysicalDeviceName(VkPhysicalDevice physicalDevice);
VkPhysicalDevice vkuGetOptimalPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

VmaAllocator vkuCreateVmaAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
void vkuDestroyVmaAllocator(VmaAllocator allocator);

typedef enum CmdPoolType {
    VKU_CMD_POOL_TYPE_GRAPHICS,
    VKU_CMD_POOL_TYPE_TRANSFER,
    VKU_CMD_POOL_TYPE_COMPUTE
} CmdPoolType;

VkCommandPool vkuCreateCmdPool(VkPhysicalDevice phy_device, VkDevice device, CmdPoolType type);
void vkuDestroyCommandPool(VkDevice device, VkCommandPool cmd_pool);

VkCommandBuffer *vkuCreateCommandBuffer(VkDevice device, uint32_t count, VkCommandPool cmdPool);
void vkuDestroyCmdBuffer(VkCommandBuffer *cmdBuffer);

typedef struct VkuVkImageCreateInfo
{
    VmaAllocator allocator;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usageFlags;
    VkSampleCountFlagBits numSamples;
    VkImage *pImage;
    VmaAllocation *pImageAlloc;
    VmaAllocationInfo *pImageAllocInfo;
} VkuVkImageCreateInfo;

void vkuCreateImage(VkuVkImageCreateInfo *createInfo);
void vkuDestroyImage(VkImage image, VmaAllocation imageAlloc, VmaAllocator allocator);

VkImageView vkuCreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipmap_levels, uint32_t layerCount, VkDevice device);
void vkuDestroyImageView(VkImageView imageView, VkDevice device);

typedef struct VkuColorResourcesCreateInfo
{
    VkFormat format;
    VmaAllocator allocator;
    VkExtent2D extend;
    VkSampleCountFlagBits msaa_samples;
    VkDevice device;
} VkuColorResourcesCreateInfo;

typedef struct VkuColorResource_T
{
    VkImage image;
    VkImageView imageView;
    VmaAllocation imageAlloc;
    VmaAllocationInfo *imageAllocInfo;
} VkuColorResource_T;

typedef struct VkuColorResource_T *VkuColorResource;

VkuColorResource vkuCreateColorResources(VkuColorResourcesCreateInfo *createInfo);
void vkuDestroyColorResources(VmaAllocator allocator, VkDevice device, VkuColorResource colorResource);

typedef struct VkuDepthResourcesCreateInfo
{
    VkPhysicalDevice physical_device;
    VkDevice device;
    VmaAllocator allocator;
    VkExtent2D extend;
    VkSampleCountFlagBits msaa_samples;
    uint32_t layerCount;
} VkuDepthResourcesCreateInfo;

typedef struct VkuDepthResource_T
{
    VkImage image;
    VkImageView imageView;
    VmaAllocation imageAlloc;
    VmaAllocationInfo *imageAllocInfo;
} VkuDepthResource_T;

typedef struct VkuDepthResource_T *VkuDepthResource;

VkFormat vkuFindSupportedFormat(VkFormat candidates[], uint32_t numCandidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);
VkFormat vkuFindDepthFormat(VkPhysicalDevice physicalDevice);
VkuDepthResource vkuCreateDepthResources(VkuDepthResourcesCreateInfo *createInfo);
void vkuDestroyDepthResources(VmaAllocator allocator, VkDevice device, VkuDepthResource depthResource);
void vkuTransitionDepthImageLayout(VkDevice device, VkCommandBuffer commandBuffer, VkImage depthImage, uint32_t layerCount);

typedef struct VkuVkSwapchainKHRCreateInfo
{
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    SDL_Window * sdlWindow;
    VkPresentModeKHR presentMode;
    uint32_t *pSwapchainImageCount;
    VkImage **ppSwapchainImages;
    VkFormat *pSwapchainFormat;
    VkExtent2D *pSwapchainExtent;
} VkuVkSwapchainKHRCreateInfo;

VkSurfaceFormatKHR vkuChooseSurfaceFormat(VkSurfaceFormatKHR *available_formats, uint32_t format_count);
VkPresentModeKHR vkuChooseSwapPresentMode(VkPresentModeKHR *present_modes, uint32_t present_mode_count, VkPresentModeKHR requested_mode);
VkExtent2D vkuChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, SDL_Window * sdlWindow);
VkSwapchainKHR vkuCreateVkSwapchainKHR(VkuVkSwapchainKHRCreateInfo *createInfo);
void vkuDestroyVkSwapchainKHR(VkSwapchainKHR swapchain, VkDevice device, VkImage *swapchain_images);
VkPresentModeKHR vkuSelectPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkPresentModeKHR requestedMode);
VkImageView *vkuCreateSwapchainImageViews(uint32_t swapchain_img_count, VkImage *swapchain_images, VkFormat swapchain_image_format, VkDevice device);
void vkuDestroySwapchainImageViews(uint32_t swapchain_img_count, VkImageView *swapchain_image_views, VkDevice device);

typedef struct VkuSyncObjectsCreateInfo
{
    VkSemaphore **ppImageAvailableSemaphores;
    VkSemaphore **ppRenderFinishedSemaphores;
    VkFence **ppInFlightFences;
    uint32_t framesInFlight;
    VkDevice device;
} VkuSyncObjectsCreateInfo;

void vkuCreateSyncObjects(VkuSyncObjectsCreateInfo *createInfo);
void vkuDestroySyncObjects(VkDevice device, uint32_t framesInFlight, VkSemaphore *ImageAvailableSemaphores, VkSemaphore *RenderFinishedSemaphores, VkFence *InFlightFences);

typedef struct VkuVkRenderPassCreateInfo
{
    VkFormat format;
    VkSampleCountFlagBits msaaSamples;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkBool32 enableDepthTest;
    VkBool32 enableTargetColorImage;
    VkBool32 enableTargetDepthImage;
    VkBool32 presentationLayout;
} VkuVkRenderPassCreateInfo;

VkRenderPass vkuCreateVkRenderPass(VkuVkRenderPassCreateInfo *createInfo);
void vkuDestroyVkRenderPass(VkDevice device, VkRenderPass renderPass);

typedef struct VkuVkFramebufferCreateInfo
{
    uint32_t imageCount;
    uint32_t layerCount;
    VkSampleCountFlagBits msaaSamples;
    VkImageView renderStageColorImageView;
    VkImageView renderStageDepthImageView;
    VkImageView *renderTargetColorImageViews;
    VkImageView *renderTargetDepthImageViews;
    VkRenderPass renderPass;
    VkExtent2D extend;
    VkDevice device;

    VkBool32 enableTargetColorImage;
    VkBool32 enableTargetDepthImage;
    VkBool32 enableDepthTest;
} VkuVkFramebufferCreateInfo;

VkFramebuffer *vkuCreateVkFramebuffer(VkuVkFramebufferCreateInfo *createInfo);
void vkuDestroyFramebuffer(VkDevice device, VkFramebuffer *framebuffer, uint32_t framebuffer_count);

typedef struct VkuTextureImageCreateInfo
{
    uint8_t *textureData;
    int texWidth, texHeight;
    uint32_t mipLevels;
    VmaAllocator allocator;
    VkImage *pTexImage;
    VmaAllocation *pTexImageAlloc;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool cmdPool;
    VkQueue graphicsQueue;
} VkuTextureImageCreateInfo;

typedef struct VkuTextureImageArrayCreateInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    uint8_t **tex_data;
    uint32_t mip_levels;
    VmaAllocator allocator;
    VkImage *pTexArrayImg;
    VmaAllocation *pTexArrayAlloc;
    VkDevice device;
    VkCommandPool cmd_pool;
    VkQueue graphics_queue;
    VkPhysicalDevice physical_device;
} VkuTextureImageArrayCreateInfo;

VkCommandBuffer vkuBeginSingleTimeCommands(VkDevice device, VkCommandPool cmd_pool);
void vkuEndAndSubmitSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer);
void vkuTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mip_levels, uint32_t layer_count, VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue);
void vkuCopyBufferToImage(VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue, VkBuffer buffer, VkImage image, int width, int height, uint32_t layer_count);
void vkuGenerateMipmaps(VkPhysicalDevice phy_device, VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue, VkFormat imageFormat, VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layer_count);
void vkuCreateTextureImage(VkuTextureImageCreateInfo *createInfo);
void vkuDestroyTextureImage(VmaAllocator allocator, VkImage texImg, VmaAllocation texImgAlloc);
VkImageView vkuCreateTextureImageView(VkDevice device, VkImage image, uint32_t mipLevels);
void vkuDestroyTextureImageView(VkDevice device, VkImageView image_view);
void vkuCreateTextureImageArray(VkuTextureImageArrayCreateInfo *create_info);
VkImageView vkuCreateTextureImageArrayView(VkDevice device, VkImage image, uint32_t mipLevels, uint32_t layerCount);

typedef struct VkuUniformBuffersCreateInfo
{
    VkBuffer **ppUniformBuffer;
    VmaAllocation **ppUniformAllocations;
    void ***pppMappedUniformMemory;
    VmaAllocator allocator;
    size_t bufferSize;
    uint32_t bufferCount;
} VkuUniformBuffersCreateInfo;

void vkuCreateUniformBuffers(VkuUniformBuffersCreateInfo *createInfo);
void vkuDestroyUniformBuffers(VmaAllocator allocator, VkBuffer *uniBuffer, VmaAllocation *uniAllocs, void **mappedUni, uint32_t bufferCount);

typedef struct VkuDescriptorSetsCreateInfo
{
    uint32_t setCount;
    VkDescriptorSetLayout setLayout;
    VkDescriptorPool pool;
    VkDevice device;
    VkuDescriptorSetAttribute *attributes;
    uint32_t attribCount;
} VkuDescriptorSetsCreateInfo;

VkDescriptorSetLayout vkuCreateDescriptorSetLayout(VkDevice device, VkuDescriptorSetAttribute *attribs, uint32_t attribCount);
void vkuDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout set_layout);
VkDescriptorPool vkuCreateDescriptorPool(VkDevice device, VkuDescriptorSetAttribute *attributes, uint32_t attribCount, uint32_t setCount);
void vkuDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool);
VkDescriptorSet *vkuCreateDescriptorSets(VkuDescriptorSetsCreateInfo *createInfo);
void vkuDestroyDescriptorSets(VkDescriptorSet *sets);

typedef struct VkuGraphicsPipelineCreateInfo
{
    VkDevice device;
    char *vertexShaderSpirv;
    uint32_t vertexShaderLength;
    char *fragmentShaderSpirv;
    uint32_t fragmentShaderLength;
    VkuVertexLayout vertexInputLayout;
    VkExtent2D swapchainExtend;
    VkPolygonMode polygonMode;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkSampleCountFlagBits msaaSamples;
    VkBool32 depth_test_write;
    VkBool32 depth_test_enable;
    VkCompareOp depth_compare_mode;
    VkCullModeFlags cullMode;
    VkBool32 enableDepthBias;
    float depthBiasConstantFactor;
    float depthBiasSlopeFactor;
    VkPrimitiveTopology topology;
} VkuGraphicsPipelineCreateInfo;

VkPipelineLayout vkuCreatePipelineLayout(VkDevice device, VkDescriptorSetLayout *setLayouts, uint32_t setLayoutCount);
void vkuDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout);
VkShaderModule vkuCreateShaderModule(const char *shaderCode, uint32_t codeLength, VkDevice device);
VkVertexInputBindingDescription vkuGetVertexInputBindingDescription(VkuVertexLayout *layout);
VkVertexInputAttributeDescription *vkuGetVertexAttributeDescriptions(VkuVertexLayout *layout);
VkPipeline vkuCreateGraphicsPipeline(VkuGraphicsPipelineCreateInfo *createInfo);
void vkuDestroyVkPipeline(VkDevice device, VkPipeline pipeline);

typedef struct VkuComputeVkPipelineCreateInfo
{
    VkDevice device;
    char *computeShaderSpirv;
    uint32_t computeShaderLength;
    VkPipelineLayout pipelineLayout;
} VkuComputeVkPipelineCreateInfo;

VkPipeline vkuCreateComputeVkPipeline(VkuComputeVkPipelineCreateInfo *createInfo);

// ===== BASIC HELPER C FUNCTIONS =====

VkuObjectManager vkuCreateObjectManager(size_t elementSize)
{
    VkuObjectManager_T *mgr = (VkuObjectManager_T *)calloc(1, sizeof(VkuObjectManager_T));
    mgr->elemSize = elementSize;
    mgr->elemCnt = 0;
    mgr->elements = NULL;

    return mgr;
}

void vkuObjectManagerAdd(VkuObjectManager objectManager, void *element)
{
    if (!objectManager || !element)
        return;

    objectManager->elements = (void **)realloc(objectManager->elements, (objectManager->elemCnt + 1) * sizeof(void *));
    if (!objectManager->elements)
    {
        return;
    }

    objectManager->elements[objectManager->elemCnt] = element;
    objectManager->elemCnt++;
}

void vkuObjectManagerRemove(VkuObjectManager objectManager, void *element)
{
    if (!objectManager || !element || objectManager->elemCnt == 0)
        return;

    for (uint32_t i = 0; i < objectManager->elemCnt; i++)
    {
        if (objectManager->elements[i] == element)
        {
            for (uint32_t j = i; j < objectManager->elemCnt - 1; j++)
            {
                objectManager->elements[j] = objectManager->elements[j + 1];
            }

            objectManager->elemCnt--;
            objectManager->elements = (void **)realloc(objectManager->elements, objectManager->elemCnt * sizeof(void *));
            return;
        }
    }
}

int vkuDestroyObjectManager(VkuObjectManager objectManager)
{
    if (!objectManager)
        return -1;

    free(objectManager->elements);
    free(objectManager);

    return 0;
}

int find_bit_position(uint32_t value)
{
    if (value == 0)
    {
        return -1;
    }

    int position = 0;

    while ((value & 1) == 0)
    {
        value >>= 1;
        position++;
    }

    return position;
}

bool vkuContains(uint32_t *array, uint32_t count, uint32_t value)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (array[i] == value)
        {
            return true;
        }
    }
    return false;
}

void vkuAddStringToArray(char ***array, uint32_t *stringCount, const char *newString)
{
    char **newArray = (char **)realloc(*array, (*stringCount + 1) * sizeof(char *));
    if (newArray == NULL)
        EXIT("Memory allocation failed\n");

    newArray[*stringCount] = (char *)malloc((strlen(newString) + 1) * sizeof(char));
    if (newArray[*stringCount] == NULL)
        EXIT("Memory allocation for string failed\n");

    strcpy(newArray[*stringCount], newString);

    *array = newArray;
    (*stringCount)++;
}

void vkuFreeStringArray(char **array, size_t size)
{
    if (array == NULL)
        return;

    for (size_t i = 0; i < size; i++)
        free(array[i]);

    free(array);
}

char *vkuReadFile(const char *filename, uint32_t *length)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        EXIT("Error opening file!\n");

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_length < 0)
    {
        perror("Error determining file length");
        fclose(file);
        return NULL;
    }

    char *buffer = (char *)malloc(file_length + 1);
    if (!buffer)
    {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, file_length, file);
    if (read_size != (size_t) file_length)
    {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_length] = '\0';
    fclose(file);
    *length = (uint32_t)file_length;

    return buffer;
}

// Debug Messenger

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        printf("%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

void vkuPopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo, VkBool32 verboseOutput)
{
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    if (verboseOutput)
        createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    else
        createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = DebugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
        func(instance, debugMessenger, pAllocator);
}

VkDebugUtilsMessengerEXT vkuCreateVkDebugMessenger(VkInstance instance, VkBool32 validation)
{
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    if (!validation)
        return debug_messenger;

    VkDebugUtilsMessengerCreateInfoEXT create_info = {};
    vkuPopulateDebugMessengerCreateInfo(&create_info, false);

    VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &create_info, NULL, &debug_messenger));
    return debug_messenger;
}

void vkuDestroyVkDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger)
{
    DestroyDebugUtilsMessengerEXT(instance, debug_messenger, NULL);
}

// VkInstance

char **vkuGetRequiredInstanceExtensions(uint32_t *pExtensionCount)
{
    char **extensions = NULL;
    *pExtensionCount = 0;

    uint32_t sdlExtensionCount = 0;
    const char **sdlExtensions = (const char **) SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    for (uint32_t i = 0; i < sdlExtensionCount; i++)
        vkuAddStringToArray(&extensions, pExtensionCount, sdlExtensions[i]);

    vkuAddStringToArray(&extensions, pExtensionCount, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pExtensionCount, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

char **vkuGetIntanceLayers(uint32_t *pLayerCount)
{
    char **layers = NULL;
    vkuAddStringToArray(&layers, pLayerCount, VKU_VALIDATION_LAYER_NAME);

    return layers;
}

bool vkuCheckLayersAvailable(uint32_t layer_count, char **layers)
{
    uint32_t available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, NULL));
    VkLayerProperties *available_layers = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

    for (uint32_t i = 0; i < layer_count; ++i)
    {
        bool layer_found = false;
        for (uint32_t j = 0; j < available_layer_count; ++j)
        {
            if (strcmp(layers[i], available_layers[j].layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }
        if (!layer_found)
        {
            fprintf(stderr, "Requested layer %s is not available.\n", layers[i]);
            free(available_layers);
            return false;
        }
    }

    free(available_layers);
    return true;
}

bool vkuCheckExtensionAvailable(uint32_t extension_count, char **extensions)
{
    uint32_t available_extension_count = 0;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, NULL));
    VkExtensionProperties *available_extensions = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * available_extension_count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &available_extension_count, available_extensions));

    for (uint32_t i = 0; i < extension_count; ++i)
    {
        bool extension_found = false;
        for (uint32_t j = 0; j < available_extension_count; ++j)
        {
            if (strcmp(extensions[i], available_extensions[j].extensionName) == 0)
            {
                extension_found = true;
                break;
            }
        }

        if (!extension_found)
        {
            fprintf(stderr, "Requested extension %s is not available.\n", extensions[i]);
            free(available_extensions);
            return false;
        }
    }

    free(available_extensions);
    return true;
}

VkInstance vkuCreateVkInstance(VkuVkInstanceCreateInfo *createInfo)
{
    uint32_t instance_extension_count = 0;
    char **instance_extensions = vkuGetRequiredInstanceExtensions(&instance_extension_count);
    uint32_t instance_layer_count = 0;
    char **instance_layers = NULL;

    if (createInfo->enableValidation)
    {
        instance_layers = vkuGetIntanceLayers(&instance_layer_count);
    }

    if (!vkuCheckExtensionAvailable(instance_extension_count, instance_extensions))
    {
        EXIT("Instance Extensions not available!\n");
    }

    if (!vkuCheckLayersAvailable(instance_layer_count, instance_layers))
    {
        EXIT("Instance Layers not available!\n");
    }

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = createInfo->applicationName,
        .applicationVersion = createInfo->applicationVersion,
        .pEngineName = VKU_ENGINE_NAME,
        .engineVersion = VKU_ENGINE_VERSION,
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = instance_layer_count,
        .ppEnabledLayerNames = (const char **)instance_layers,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = (const char **)instance_extensions,
    };

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (createInfo->enableValidation)
    {
        vkuPopulateDebugMessengerCreateInfo(&debugCreateInfo, false);
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }

    VkInstance instance = VK_NULL_HANDLE;
    VK_CHECK(vkCreateInstance(&instanceInfo, NULL, &instance));

    vkuFreeStringArray(instance_extensions, instance_extension_count);
    vkuFreeStringArray(instance_layers, instance_layer_count);

    return instance;
}

void vkuDestroyVkInstance(VkInstance instance)
{
    vkDestroyInstance(instance, NULL);
}

// VkSurface

VkSurfaceKHR vkuCreateSurface(VkInstance instance, SDL_Window * sdlWindow)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    SDL_Vulkan_CreateSurface(sdlWindow, instance, NULL, &surface);
    return surface;
}

void vkuDestroySurface(VkSurfaceKHR surface, VkInstance instance)
{
    vkDestroySurfaceKHR(instance, surface, NULL);
}

// VkDevice

char **vkuGetDeviceExtensions(uint32_t *pDeviceExtensionCount)
{
    char **extensions = NULL;
    (*pDeviceExtensionCount) = 0;

    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
    vkuAddStringToArray(&extensions, pDeviceExtensionCount, VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);

    return extensions;
}

void vkuGetQueueFamilyIndices(VkuQueueFamilyIndices *indices, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    indices->computeQueueFam = UINT32_MAX;
    indices->graphicsQueueFam = UINT32_MAX;
    indices->presentQueueFam = UINT32_MAX;
    indices->transferQueueFam = UINT32_MAX;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    bool idealTransferQueueFound = false;
    bool preferDiffrentQueueFamilies = true;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        VkBool32 presentSupport = VK_FALSE;

        if (surface != VK_NULL_HANDLE)
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices->graphicsQueueFam = i;
            if (surface != VK_NULL_HANDLE && presentSupport)
                indices->presentQueueFam = i;
        }

        if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
            !(queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) &&
            !idealTransferQueueFound)
        {
            indices->transferQueueFam = i;
            idealTransferQueueFound = true;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !idealTransferQueueFound)
            indices->transferQueueFam = i;

        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices->computeQueueFam = i;

        if (indices->computeQueueFam != UINT32_MAX && indices->graphicsQueueFam != UINT32_MAX &&
            (surface == VK_NULL_HANDLE || indices->presentQueueFam != UINT32_MAX) &&
            indices->transferQueueFam != UINT32_MAX && !preferDiffrentQueueFamilies)
            break;
    }

    free(queueFamilies);
}

void vkuGetUniqueQueueFamilyIndices(VkuQueueFamilyIndices *indices, uint32_t *unique_queue_family_count, uint32_t unique_queue_families[4])
{
    *unique_queue_family_count = 0;

    if (!vkuContains(unique_queue_families, *unique_queue_family_count, indices->graphicsQueueFam) && indices->graphicsQueueFam != UINT32_MAX)
    {
        unique_queue_families[*unique_queue_family_count] = indices->graphicsQueueFam;
        (*unique_queue_family_count)++;
    }

    if (!vkuContains(unique_queue_families, *unique_queue_family_count, indices->presentQueueFam) && indices->presentQueueFam != UINT32_MAX)
    {
        unique_queue_families[*unique_queue_family_count] = indices->presentQueueFam;
        (*unique_queue_family_count)++;
    }

    if (!vkuContains(unique_queue_families, *unique_queue_family_count, indices->transferQueueFam) && indices->transferQueueFam != UINT32_MAX)
    {
        unique_queue_families[*unique_queue_family_count] = indices->transferQueueFam;
        (*unique_queue_family_count)++;
    }

    if (!vkuContains(unique_queue_families, *unique_queue_family_count, indices->computeQueueFam) && indices->computeQueueFam != UINT32_MAX)
    {
        unique_queue_families[*unique_queue_family_count] = indices->computeQueueFam;
        (*unique_queue_family_count)++;
    }
}

VkDevice vkuCreateVkDevice(VkuVkDeviceCreateInfo *create_info)
{
    VkDevice device = VK_NULL_HANDLE;

    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.pNext = NULL;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &bufferDeviceAddressFeatures;

    vkGetPhysicalDeviceFeatures2(create_info->physical_device, &deviceFeatures2);

    VkuQueueFamilyIndices indices = {};
    vkuGetQueueFamilyIndices(&indices, create_info->physical_device, create_info->surface);

    uint32_t unique_queue_family_count = 0;
    uint32_t unique_queue_families[4] = {};
    vkuGetUniqueQueueFamilyIndices(&indices, &unique_queue_family_count, unique_queue_families);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfos[unique_queue_family_count];
    for (uint32_t i = 0; i < unique_queue_family_count; i++)
    {
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].pNext = NULL;
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = unique_queue_families[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = unique_queue_family_count;
    createInfo.pEnabledFeatures = &deviceFeatures;

    if (bufferDeviceAddressFeatures.bufferDeviceAddress)
        createInfo.pNext = &bufferDeviceAddressFeatures;

    uint32_t device_extension_count = 0;
    char **device_extensions = vkuGetDeviceExtensions(&device_extension_count);
    createInfo.enabledExtensionCount = device_extension_count;
    createInfo.ppEnabledExtensionNames = (const char **)device_extensions;

    uint32_t layer_count = 0;
    char **layers = NULL;

    if (create_info->enable_validation)
        layers = vkuGetIntanceLayers(&layer_count);

    createInfo.enabledLayerCount = layer_count;
    createInfo.ppEnabledLayerNames = (const char **)layers;

    VK_CHECK(vkCreateDevice(create_info->physical_device, &createInfo, NULL, &device));

    vkuFreeStringArray(device_extensions, device_extension_count);
    vkuFreeStringArray(layers, layer_count);

    if (create_info->pGraphicsQueue != NULL)
    {
        vkGetDeviceQueue(device, indices.graphicsQueueFam, 0, create_info->pGraphicsQueue);
    }

    if (create_info->pTransferQueue != NULL)
        vkGetDeviceQueue(device, indices.transferQueueFam, 0, create_info->pTransferQueue);

    if (create_info->surface != VK_NULL_HANDLE && indices.presentQueueFam != UINT32_MAX && create_info->pPresentQueue != NULL)
        vkGetDeviceQueue(device, indices.presentQueueFam, 0, create_info->pPresentQueue);

    if (create_info->pComputeQueue != NULL)
        vkGetDeviceQueue(device, indices.computeQueueFam, 0, create_info->pComputeQueue);

    return device;
}

void vkuDestroyVkDevice(VkDevice device)
{
    vkDestroyDevice(device, NULL);
}

// VkPhysicalDevice

VkSampleCountFlagBits vkuGetMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT)
        return VK_SAMPLE_COUNT_64_BIT;

    if (counts & VK_SAMPLE_COUNT_32_BIT)
        return VK_SAMPLE_COUNT_32_BIT;

    if (counts & VK_SAMPLE_COUNT_16_BIT)
        return VK_SAMPLE_COUNT_16_BIT;

    if (counts & VK_SAMPLE_COUNT_8_BIT)
        return VK_SAMPLE_COUNT_8_BIT;

    if (counts & VK_SAMPLE_COUNT_4_BIT)
        return VK_SAMPLE_COUNT_4_BIT;

    if (counts & VK_SAMPLE_COUNT_2_BIT)
        return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

void vkuQuerySwapchainSupport(VkuSwapchainSupportDetails *details, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    if (surface == VK_NULL_HANDLE)
    {
        details->formats = NULL;
        details->formatsCount = 0;
        details->presentModes = NULL;
        details->presentModesCount = 0;
        return;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, NULL);

    if (formatCount != 0)
    {
        details->formats = (VkSurfaceFormatKHR *)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
        if (details->formats)
        {
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, details->formats);
            details->formatsCount = formatCount;
        }
        else
        {
            details->formatsCount = 0;
        }
    }
    else
    {
        details->formats = NULL;
        details->formatsCount = 0;
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, NULL);

    if (presentModeCount != 0)
    {
        details->presentModes = (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
        if (details->presentModes)
        {
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, details->presentModes);
            details->presentModesCount = presentModeCount;
        }
        else
        {
            details->presentModesCount = 0;
        }
    }
    else
    {
        details->presentModes = NULL;
        details->presentModesCount = 0;
    }
}

void vkuDestroySwapchainSupportDetails(VkuSwapchainSupportDetails *details)
{
    free(details->formats);
    free(details->presentModes);
}

bool vkuCheckPhysicalDeviceExtensionSupport(VkPhysicalDevice physical_device, char **deviceExtensions, uint32_t deviceExtensionCount)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);
    VkExtensionProperties *available_extensions = (VkExtensionProperties *)malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, available_extensions);

    bool *extension_support = (bool *)malloc(deviceExtensionCount * sizeof(bool));
    memset(extension_support, 0, deviceExtensionCount * sizeof(bool));

    for (uint32_t i = 0; i < extension_count; ++i)
    {
        for (uint32_t j = 0; j < deviceExtensionCount; ++j)
        {
            if (strcmp(deviceExtensions[j], available_extensions[i].extensionName) == 0)
            {
                extension_support[j] = true;
                break;
            }
        }
    }

    bool all_supported = true;
    for (uint32_t i = 0; i < deviceExtensionCount; ++i)
    {
        if (!extension_support[i])
        {
            all_supported = false;
            break;
        }
    }

    free(available_extensions);
    free(extension_support);

    return all_supported;
}

bool vkuComparePhysicalDevices(VkPhysicalDevice device1, VkPhysicalDevice device2)
{
    VkPhysicalDeviceProperties properties1, properties2;

    vkGetPhysicalDeviceProperties(device1, &properties1);
    vkGetPhysicalDeviceProperties(device2, &properties2);

    return (properties1.deviceID == properties2.deviceID &&
            properties1.vendorID == properties2.vendorID &&
            properties1.deviceType == properties2.deviceType);
}

/**
 * @param surface can be VK_NULL_HANDLE. It disables checking for swapchain and present support!
 * @return Returned array must be freed manually!
 */

VkPhysicalDevice *vkuGetSuitableVkPhysicalDevices(uint32_t *pPhysicalDeviceCount, VkSurfaceKHR surface, VkInstance instance)
{
    VkPhysicalDevice *suitablePhysicalDevices = NULL;
    (*pPhysicalDeviceCount) = 0;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0)
        EXIT("Failed to find GPUs with Vulkan support!\n");

    VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physical_devices);

    uint32_t device_ext_count = 0;
    char **device_ext = vkuGetDeviceExtensions(&device_ext_count);

    for (uint32_t i = 0; i < deviceCount; i++)
    {
        VkuQueueFamilyIndices indices = {};
        vkuGetQueueFamilyIndices(&indices, physical_devices[i], surface);

        VkuSwapchainSupportDetails details = {};
        vkuQuerySwapchainSupport(&details, physical_devices[i], surface);

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &supportedFeatures);

        bool device_extensions_supported = vkuCheckPhysicalDeviceExtensionSupport(physical_devices[i], device_ext, device_ext_count);
        bool found_queue_families =
            indices.graphicsQueueFam != UINT32_MAX &&
            indices.computeQueueFam != UINT32_MAX &&
            indices.transferQueueFam != UINT32_MAX;

        bool presentAndSwapchain = true;
        if (surface != VK_NULL_HANDLE)
        {
            presentAndSwapchain = (indices.presentQueueFam != UINT32_MAX) &&
                                  (details.formats != NULL && details.presentModes != NULL);
        }

        bool features = supportedFeatures.samplerAnisotropy;

        if (device_extensions_supported && found_queue_families && presentAndSwapchain && features)
        {
            (*pPhysicalDeviceCount)++;
            suitablePhysicalDevices = (VkPhysicalDevice *)realloc(suitablePhysicalDevices, sizeof(VkPhysicalDevice) * (*pPhysicalDeviceCount));
            if (!suitablePhysicalDevices)
            {
                vkuDestroySwapchainSupportDetails(&details);
                free(physical_devices);
                vkuFreeStringArray(device_ext, device_ext_count);
                EXIT("Failed to allocate memory for suitable physical devices!\n");
            }
            suitablePhysicalDevices[(*pPhysicalDeviceCount) - 1] = physical_devices[i];
        }

        vkuDestroySwapchainSupportDetails(&details);
    }

    free(physical_devices);
    vkuFreeStringArray(device_ext, device_ext_count);

    return suitablePhysicalDevices;
}

void vkuPrintVkPhysicalDeviceName(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    printf("\x1b[35m%s\x1b[0m\n", deviceProperties.deviceName);
}

VkPhysicalDevice vkuGetOptimalPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t phy_count = 0;
    VkPhysicalDevice *phys = vkuGetSuitableVkPhysicalDevices(&phy_count, surface, instance);
    uint32_t phy_scores[phy_count];

    for (uint32_t i = 0; i < phy_count; i++)
    {
        uint32_t score = 0;

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(phys[i], &deviceProperties);
        vkGetPhysicalDeviceFeatures(phys[i], &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            score += 500;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        phy_scores[i] = score;
    }

    VkPhysicalDevice optimal_device = VK_NULL_HANDLE;

    uint32_t hightscore = 0;
    for (uint32_t i = 0; i < phy_count; i++)
    {
        if (phy_scores[i] > hightscore)
        {
            hightscore = phy_scores[i];
            optimal_device = phys[i];
        }
    }

    free(phys);
    return optimal_device;
}

// VmaAllocator

VmaAllocator vkuCreateVmaAllocator(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
{
    VmaAllocator allocator = VK_NULL_HANDLE;

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT | VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorCreateInfo.physicalDevice = physicalDevice;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &allocator));
    return allocator;
}

void vkuDestroyVmaAllocator(VmaAllocator allocator)
{
    vmaDestroyAllocator(allocator);
}

// VkCommandPool

VkCommandPool vkuCreateCmdPool(VkPhysicalDevice phy_device, VkDevice device, CmdPoolType type)
{
    VkCommandPool cmd_pool = VK_NULL_HANDLE;

    VkuQueueFamilyIndices indices = {};
    vkuGetQueueFamilyIndices(&indices, phy_device, NULL);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (type == VKU_CMD_POOL_TYPE_TRANSFER)
        poolInfo.queueFamilyIndex = indices.transferQueueFam;
    else if (type == VKU_CMD_POOL_TYPE_GRAPHICS)
        poolInfo.queueFamilyIndex = indices.graphicsQueueFam;
    else if (type == VKU_CMD_POOL_TYPE_COMPUTE)
        poolInfo.queueFamilyIndex = indices.computeQueueFam;

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, NULL, &cmd_pool));

    return cmd_pool;
}

void vkuDestroyCommandPool(VkDevice device, VkCommandPool cmd_pool)
{
    vkDestroyCommandPool(device, cmd_pool, NULL);
}

// VkCommandBuffer

VkCommandBuffer *vkuCreateCommandBuffer(VkDevice device, uint32_t count, VkCommandPool cmdPool)
{
    VkCommandBuffer *cmdBuffer = (VkCommandBuffer *)malloc(sizeof(VkCommandBuffer) * count);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)count;

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, cmdBuffer));
    return cmdBuffer;
}

void vkuDestroyCmdBuffer(VkCommandBuffer *cmdBuffer)
{
    free(cmdBuffer);
}

// VkImage

void vkuCreateImage(VkuVkImageCreateInfo *createInfo)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = createInfo->format,
        .extent = {createInfo->width, createInfo->height, 1},
        .mipLevels = createInfo->mipLevels,
        .arrayLayers = createInfo->arrayLayers,
        .samples = createInfo->numSamples,
        .tiling = createInfo->tiling,
        .usage = createInfo->usageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VmaAllocationCreateInfo allocInfo = {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY};

    VK_CHECK(vmaCreateImage(createInfo->allocator, &imageInfo, &allocInfo, createInfo->pImage, createInfo->pImageAlloc, createInfo->pImageAllocInfo));
}

void vkuDestroyImage(VkImage image, VmaAllocation imageAlloc, VmaAllocator allocator)
{
    vmaDestroyImage(allocator, image, imageAlloc);
}

// VkImageView

VkImageView vkuCreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipmap_levels, uint32_t layerCount, VkDevice device)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = (layerCount > 1) ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipmap_levels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    VkImageView image_view = VK_NULL_HANDLE;
    VK_CHECK(vkCreateImageView(device, &viewInfo, NULL, &image_view));

    return image_view;
}

void vkuDestroyImageView(VkImageView imageView, VkDevice device)
{
    vkDestroyImageView(device, imageView, NULL);
}

// Color & Depth

VkuColorResource vkuCreateColorResources(VkuColorResourcesCreateInfo *createInfo)
{
    VkuColorResource_T *colorResource = (VkuColorResource_T *)calloc(1, sizeof(VkuColorResource_T));
    VkFormat colorFormat = createInfo->format;

    VkuVkImageCreateInfo img_info = {
        .allocator = createInfo->allocator,
        .width = createInfo->extend.width,
        .height = createInfo->extend.height,
        .mipLevels = 1,
        .format = createInfo->format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usageFlags = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .numSamples = createInfo->msaa_samples,
        .pImage = &colorResource->image,
        .pImageAlloc = &colorResource->imageAlloc,
        .pImageAllocInfo = NULL,
        .arrayLayers = 1,
    };

    vkuCreateImage(&img_info);
    colorResource->imageView = vkuCreateImageView(colorResource->image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, createInfo->device);
    return colorResource;
}

void vkuDestroyColorResources(VmaAllocator allocator, VkDevice device, VkuColorResource colorResource)
{
    vmaDestroyImage(allocator, colorResource->image, colorResource->imageAlloc);
    vkDestroyImageView(device, colorResource->imageView, NULL);
    free(colorResource);
}

VkFormat vkuFindSupportedFormat(VkFormat candidates[], uint32_t numCandidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice)
{
    for (uint32_t i = 0; i < numCandidates; i++)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return candidates[i];
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return candidates[i];
    }

    EXIT("failed to find supported format!\n");
    return (VkFormat)VK_FORMAT_UNDEFINED;
}

VkFormat vkuFindDepthFormat(VkPhysicalDevice physicalDevice)
{
    VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    VkFormat findFormat = vkuFindSupportedFormat(formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice);
    return findFormat;
}

VkuDepthResource vkuCreateDepthResources(VkuDepthResourcesCreateInfo *createInfo)
{
    VkuDepthResource_T *depthResource = (VkuDepthResource_T *)calloc(1, sizeof(VkuDepthResource_T));

    VkFormat depth_format = vkuFindDepthFormat(createInfo->physical_device);

    VkuVkImageCreateInfo image_info = {
        .allocator = createInfo->allocator,
        .width = createInfo->extend.width,
        .height = createInfo->extend.height,
        .mipLevels = 1,
        .format = depth_format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .numSamples = createInfo->msaa_samples,
        .pImage = &depthResource->image,
        .pImageAlloc = &depthResource->imageAlloc,
        .pImageAllocInfo = NULL,
        .arrayLayers = createInfo->layerCount
    };

    vkuCreateImage(&image_info);
    depthResource->imageView = vkuCreateImageView(depthResource->image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1, createInfo->layerCount, createInfo->device);
    return depthResource;
}

void vkuDestroyDepthResources(VmaAllocator allocator, VkDevice device, VkuDepthResource depthResource)
{
    vmaDestroyImage(allocator, depthResource->image, depthResource->imageAlloc);
    vkDestroyImageView(device, depthResource->imageView, NULL);
    free(depthResource);
}

void vkuTransitionDepthImageLayout(VkDevice device, VkCommandBuffer commandBuffer, VkImage depthImage, uint32_t layerCount)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = depthImage;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    VkFormat depthImageFormat = VK_FORMAT_D32_SFLOAT;
    if (depthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT)
    {
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

// VkSwapchainKHR

VkSurfaceFormatKHR vkuChooseSurfaceFormat(VkSurfaceFormatKHR *available_formats, uint32_t format_count)
{
    for (uint32_t i = 0; i < format_count; i++)
    {
        if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return available_formats[i];
    }

    return available_formats[0];
}

VkPresentModeKHR vkuChooseSwapPresentMode(VkPresentModeKHR *present_modes, uint32_t present_mode_count, VkPresentModeKHR requested_mode)
{
    for (uint32_t i = 0; i < present_mode_count; i++)
    {
        if (present_modes[i] == requested_mode)
            return present_modes[i];
    }

    return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

VkExtent2D vkuChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, SDL_Window * sdlWindow)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    int width, height;
    SDL_GetWindowSizeInPixels(sdlWindow, &width, &height);

    VkExtent2D actual_extent = {
        .width = (uint32_t)width,
        .height = (uint32_t)height,
    };

    actual_extent.width = CLAMP(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = CLAMP(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}

VkSwapchainKHR vkuCreateVkSwapchainKHR(VkuVkSwapchainKHRCreateInfo *createInfo)
{
    VkuSwapchainSupportDetails swapchain_details = {};
    vkuQuerySwapchainSupport(&swapchain_details, createInfo->physicalDevice, createInfo->surface);

    VkSurfaceFormatKHR surfaceFormat = vkuChooseSurfaceFormat(swapchain_details.formats, swapchain_details.formatsCount);
    VkPresentModeKHR presentMode = vkuChooseSwapPresentMode(swapchain_details.presentModes, swapchain_details.presentModesCount, createInfo->presentMode);
    VkExtent2D extent = vkuChooseSwapExtent(swapchain_details.capabilities, createInfo->sdlWindow);

    (*createInfo->pSwapchainExtent) = extent;
    (*createInfo->pSwapchainFormat) = surfaceFormat.format;

    uint32_t imageCount = swapchain_details.capabilities.minImageCount;
    if (swapchain_details.capabilities.maxImageCount > 0 && imageCount > swapchain_details.capabilities.maxImageCount)
        imageCount = swapchain_details.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = createInfo->surface;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkuQueueFamilyIndices indices = {};
    vkuGetQueueFamilyIndices(&indices, createInfo->physicalDevice, createInfo->surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsQueueFam, indices.presentQueueFam};

    if (indices.graphicsQueueFam != indices.presentQueueFam)
    {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = NULL;
    }

    swapchainInfo.preTransform = swapchain_details.capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VK_CHECK(vkCreateSwapchainKHR(createInfo->device, &swapchainInfo, NULL, &swapchain));

    vkGetSwapchainImagesKHR(createInfo->device, swapchain, createInfo->pSwapchainImageCount, NULL);
    (*createInfo->ppSwapchainImages) = (VkImage *)malloc(sizeof(VkImage) * (*createInfo->pSwapchainImageCount));
    vkGetSwapchainImagesKHR(createInfo->device, swapchain, createInfo->pSwapchainImageCount, (*createInfo->ppSwapchainImages));

    vkuDestroySwapchainSupportDetails(&swapchain_details);
    return swapchain;
}

void vkuDestroyVkSwapchainKHR(VkSwapchainKHR swapchain, VkDevice device, VkImage *swapchain_images)
{
    vkDestroySwapchainKHR(device, swapchain, NULL);
    free(swapchain_images);
}

VkPresentModeKHR vkuSelectPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkPresentModeKHR requestedMode)
{
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

    VkPresentModeKHR availablePresentModes[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, availablePresentModes);

    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (availablePresentModes[i] == requestedMode)
        {
            return requestedMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkImageView *vkuCreateSwapchainImageViews(uint32_t swapchain_img_count, VkImage *swapchain_images, VkFormat swapchain_image_format, VkDevice device)
{
    VkImageView *image_views = (VkImageView *)malloc(sizeof(VkImageView) * swapchain_img_count);

    for (uint32_t i = 0; i < swapchain_img_count; i++)
        image_views[i] = vkuCreateImageView(swapchain_images[i], swapchain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, device);

    return image_views;
}

void vkuDestroySwapchainImageViews(uint32_t swapchain_img_count, VkImageView *swapchain_image_views, VkDevice device)
{
    for (uint32_t i = 0; i < swapchain_img_count; i++)
        vkuDestroyImageView(swapchain_image_views[i], device);

    free(swapchain_image_views);
}

// Vulkan Sync Objects

void vkuCreateSyncObjects(VkuSyncObjectsCreateInfo *createInfo)
{
    (*createInfo->ppImageAvailableSemaphores) = (VkSemaphore *)realloc(*createInfo->ppImageAvailableSemaphores, sizeof(VkSemaphore) * createInfo->framesInFlight);
    (*createInfo->ppRenderFinishedSemaphores) = (VkSemaphore *)realloc(*createInfo->ppRenderFinishedSemaphores, sizeof(VkSemaphore) * createInfo->framesInFlight);
    (*createInfo->ppInFlightFences) = (VkFence *)realloc(*createInfo->ppInFlightFences, sizeof(VkFence) * createInfo->framesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < createInfo->framesInFlight; i++)
    {
        VK_CHECK(vkCreateSemaphore(createInfo->device, &semaphoreInfo, NULL, &(*createInfo->ppImageAvailableSemaphores)[i]));
        VK_CHECK(vkCreateSemaphore(createInfo->device, &semaphoreInfo, NULL, &(*createInfo->ppRenderFinishedSemaphores)[i]));
        VK_CHECK(vkCreateFence(createInfo->device, &fenceInfo, NULL, &(*createInfo->ppInFlightFences)[i]));
    }
}

void vkuDestroySyncObjects(VkDevice device, uint32_t framesInFlight, VkSemaphore *ImageAvailableSemaphores, VkSemaphore *RenderFinishedSemaphores, VkFence *InFlightFences)
{
    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        vkDestroySemaphore(device, RenderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(device, ImageAvailableSemaphores[i], NULL);
        vkDestroyFence(device, InFlightFences[i], NULL);
    }

    free(ImageAvailableSemaphores);
    free(RenderFinishedSemaphores);
    free(InFlightFences);
}

// VkRenderPass

VkRenderPass vkuCreateVkRenderPass(VkuVkRenderPassCreateInfo *createInfo)
{
    VkRenderPass renderPass = VK_NULL_HANDLE;

    VkImageLayout finalLayout = createInfo->presentationLayout ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription2 attachments[4];
    uint32_t attachmentCount = 0;

    VkAttachmentReference2 colorAttachmentRef = {};
    VkAttachmentReference2 depthAttachmentRef = {};
    VkAttachmentReference2 colorAttachmentResolveRef = {};
    VkAttachmentReference2 depthAttachmentResolveRef = {};

    // Color attachment
    if (createInfo->enableTargetColorImage)
    {
        VkAttachmentDescription2 colorAttachment = {};
        colorAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        colorAttachment.format = createInfo->format;
        colorAttachment.samples = createInfo->msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        else
            colorAttachment.finalLayout = finalLayout;

        attachments[attachmentCount] = colorAttachment;
        colorAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        colorAttachmentRef.attachment = attachmentCount++;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // Depth attachment
    if (createInfo->enableDepthTest)
    {
        VkAttachmentDescription2 depthAttachment = {};
        depthAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        depthAttachment.format = vkuFindDepthFormat(createInfo->physicalDevice);
        depthAttachment.samples = createInfo->msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments[attachmentCount] = depthAttachment;
        depthAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        depthAttachmentRef.attachment = attachmentCount++;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // Color resolve attachment
    if (createInfo->enableTargetColorImage && createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentDescription2 colorAttachmentResolve = {};
        colorAttachmentResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        colorAttachmentResolve.format = createInfo->format;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = finalLayout;

        attachments[attachmentCount] = colorAttachmentResolve;
        colorAttachmentResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        colorAttachmentResolveRef.attachment = attachmentCount++;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // Depth resolve attachment
    if (createInfo->enableTargetDepthImage && createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentDescription2 depthAttachmentResolve = {};
        depthAttachmentResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
        depthAttachmentResolve.format = vkuFindDepthFormat(createInfo->physicalDevice);
        depthAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        attachments[attachmentCount] = depthAttachmentResolve;
        depthAttachmentResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
        depthAttachmentResolveRef.attachment = attachmentCount++;
        depthAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // Subpass
    VkSubpassDescription2 subpass = {};
    subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    if (createInfo->enableTargetColorImage) {
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
    } else {
        subpass.colorAttachmentCount = 0;
        subpass.pColorAttachments = NULL; // Explicitly no color attachments
    }

    if (createInfo->enableDepthTest)
    {
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
    }

    VkSubpassDescriptionDepthStencilResolve depthResolveInfo = {};
    depthResolveInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
    depthResolveInfo.depthResolveMode = VK_RESOLVE_MODE_MAX_BIT;
    depthResolveInfo.stencilResolveMode = VK_RESOLVE_MODE_NONE;
    depthResolveInfo.pDepthStencilResolveAttachment = &depthAttachmentResolveRef;

    if (createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
    {
        if (createInfo->enableTargetColorImage)
        {
            subpass.pResolveAttachments = &colorAttachmentResolveRef;
        }

        if (createInfo->enableTargetDepthImage)
        {
            subpass.pNext = &depthResolveInfo;
        }
    }

    // Dependency
    VkSubpassDependency2 dependency = {};
    dependency.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    if (createInfo->enableTargetColorImage)
    {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    // Render pass info
    VkRenderPassCreateInfo2 renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
    renderPassInfo.attachmentCount = attachmentCount;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass2(createInfo->device, &renderPassInfo, NULL, &renderPass));

    return renderPass;
}

void vkuDestroyVkRenderPass(VkDevice device, VkRenderPass renderPass)
{
    vkDestroyRenderPass(device, renderPass, NULL);
}

// VkFrameBuffer

VkFramebuffer *vkuCreateVkFramebuffer(VkuVkFramebufferCreateInfo *createInfo)
{
    VkFramebuffer *framebuffers = (VkFramebuffer *)malloc(sizeof(VkFramebuffer) * createInfo->imageCount);

    for (uint32_t i = 0; i < createInfo->imageCount; i++)
    {
        uint32_t attachmentCount = 0;
        VkImageView attachments[4] = {};

        if (createInfo->enableTargetColorImage)
        {
            if (createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
            {
                attachments[attachmentCount++] = createInfo->renderStageColorImageView;
            }
            else
            {
                attachments[attachmentCount++] = createInfo->renderTargetColorImageViews[i];
            }
        }

        if (createInfo->enableDepthTest)
        {
            if (createInfo->msaaSamples == VK_SAMPLE_COUNT_1_BIT && createInfo->enableTargetDepthImage)
            {
                attachments[attachmentCount++] = createInfo->renderTargetDepthImageViews[0];
            }
            else if (createInfo->renderStageDepthImageView != VK_NULL_HANDLE)
            {
                attachments[attachmentCount++] = createInfo->renderStageDepthImageView;
            }
        }

        if (createInfo->enableTargetColorImage && createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            attachments[attachmentCount++] = createInfo->renderTargetColorImageViews[i];
        }

        if (createInfo->enableTargetDepthImage && createInfo->msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            attachments[attachmentCount++] = createInfo->renderTargetDepthImageViews[0];
        }

        // Ensure attachment count doesn't exceed array bounds
        if (attachmentCount > 4)
        {
            fprintf(stderr, "Error: Too many attachments for framebuffer!\n");
            free(framebuffers);
            return NULL;
        }

        // Framebuffer creation info
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = createInfo->renderPass;
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = createInfo->extend.width;
        framebufferInfo.height = createInfo->extend.height;
        framebufferInfo.layers = createInfo->layerCount;

        // Create the framebuffer
        VkResult result = vkCreateFramebuffer(createInfo->device, &framebufferInfo, NULL, &framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            fprintf(stderr, "Failed to create framebuffer for image %d\n", i);
            free(framebuffers);
            return NULL;
        }
    }

    return framebuffers;
}

void vkuDestroyFramebuffer(VkDevice device, VkFramebuffer *framebuffer, uint32_t framebuffer_count)
{
    for (uint32_t i = 0; i < framebuffer_count; i++)
        vkDestroyFramebuffer(device, framebuffer[i], NULL);

    free(framebuffer);
}

// VkImage (Texture 2D & 2DArray)

VkCommandBuffer vkuBeginSingleTimeCommands(VkDevice device, VkCommandPool cmd_pool)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = cmd_pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void vkuEndAndSubmitSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void vkuTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mip_levels, uint32_t layer_count, VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue)
{
    VkCommandBuffer cmd_buffer = vkuBeginSingleTimeCommands(device, cmd_pool);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layer_count;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
        EXIT("Unsupported Image Layout Transition!\n");

    vkCmdPipelineBarrier(cmd_buffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    vkuEndAndSubmitSingleTimeCommands(device, cmd_pool, graphics_queue, cmd_buffer);
}

void vkuCopyBufferToImage(VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue, VkBuffer buffer, VkImage image, int width, int height, uint32_t layer_count)
{
    VkCommandBuffer commandBuffer = vkuBeginSingleTimeCommands(device, cmd_pool);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layer_count;

    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;

    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    vkuEndAndSubmitSingleTimeCommands(device, cmd_pool, graphics_queue, commandBuffer);
}

void vkuGenerateMipmaps(VkPhysicalDevice phy_device, VkDevice device, VkCommandPool cmd_pool, VkQueue graphics_queue, VkFormat imageFormat, VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layer_count)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(phy_device, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        EXIT("texture image format does not support linear blitting!\n");

    VkCommandBuffer commandBuffer = vkuBeginSingleTimeCommands(device, cmd_pool);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layer_count;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        VkImageBlit blit = {0};
        blit.srcOffsets[0].x = 0;
        blit.srcOffsets[0].y = 0;
        blit.srcOffsets[0].z = 0;
        blit.srcOffsets[1].x = mipWidth;
        blit.srcOffsets[1].y = mipHeight;
        blit.srcOffsets[1].z = 1;
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = layer_count;
        blit.dstOffsets[0].x = 0;
        blit.dstOffsets[0].y = 0;
        blit.dstOffsets[0].z = 0;
        blit.dstOffsets[1].x = mipWidth > 1 ? mipWidth / 2 : 1;
        blit.dstOffsets[1].y = mipHeight > 1 ? mipHeight / 2 : 1;
        blit.dstOffsets[1].z = 1;
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = layer_count;

        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
    vkuEndAndSubmitSingleTimeCommands(device, cmd_pool, graphics_queue, commandBuffer);
}

void vkuCreateTextureImage(VkuTextureImageCreateInfo *createInfo)
{
    if (createInfo->textureData == NULL || createInfo->texHeight <= 0 || createInfo->texWidth <= 0)
        EXIT("Input Image Data was empty or dimensions are <= 0!\n");

    VkDeviceSize img_size = createInfo->texHeight * createInfo->texWidth * 4;

    VkBuffer staging_buffer;
    VmaAllocation staging_buffer_mem;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = img_size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VK_CHECK(vmaCreateBuffer(createInfo->allocator, &bufferInfo, &allocInfo, &staging_buffer, &staging_buffer_mem, NULL));

    void *data;
    vmaMapMemory(createInfo->allocator, staging_buffer_mem, &data);
    memcpy(data, createInfo->textureData, (size_t)img_size);
    vmaUnmapMemory(createInfo->allocator, staging_buffer_mem);

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = createInfo->texWidth;
    imageCreateInfo.extent.height = createInfo->texHeight;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = createInfo->mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VK_CHECK(vmaCreateImage(createInfo->allocator, &imageCreateInfo, &allocCreateInfo, createInfo->pTexImage, createInfo->pTexImageAlloc, NULL));

    vkuTransitionImageLayout(*createInfo->pTexImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, createInfo->mipLevels, 1, createInfo->device, createInfo->cmdPool, createInfo->graphicsQueue);
    vkuCopyBufferToImage(createInfo->device, createInfo->cmdPool, createInfo->graphicsQueue, staging_buffer, *createInfo->pTexImage, createInfo->texWidth, createInfo->texHeight, 1);
    vkuGenerateMipmaps(createInfo->physicalDevice, createInfo->device, createInfo->cmdPool, createInfo->graphicsQueue, VK_FORMAT_R8G8B8A8_SRGB, *createInfo->pTexImage, createInfo->texWidth, createInfo->texHeight, createInfo->mipLevels, 1);

    vmaDestroyBuffer(createInfo->allocator, staging_buffer, staging_buffer_mem);
}

void vkuDestroyTextureImage(VmaAllocator allocator, VkImage texImg, VmaAllocation texImgAlloc)
{
    vmaDestroyImage(allocator, texImg, texImgAlloc);
}

VkImageView vkuCreateTextureImageView(VkDevice device, VkImage image, uint32_t mipLevels)
{
    return vkuCreateImageView(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 1, device);
}

void vkuDestroyTextureImageView(VkDevice device, VkImageView image_view)
{
    vkDestroyImageView(device, image_view, NULL);
}

void vkuCreateTextureImageArray(VkuTextureImageArrayCreateInfo *create_info)
{
    VkDeviceSize image_size = create_info->width * create_info->height * 4;

    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = image_size * create_info->layers;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo stagingAllocCreateInfo = {};
    stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VK_CHECK(vmaCreateBuffer(create_info->allocator, &bufferCreateInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingBufferAllocation, NULL));

    void *data;
    vmaMapMemory(create_info->allocator, stagingBufferAllocation, &data);

    for (uint32_t i = 0; i < create_info->layers; i++)
    {
        memcpy((uint8_t *)data + i * image_size, create_info->tex_data[i], image_size);
    }

    vmaUnmapMemory(create_info->allocator, stagingBufferAllocation);

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = create_info->width;
    imageCreateInfo.extent.height = create_info->height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = create_info->mip_levels;
    imageCreateInfo.arrayLayers = create_info->layers;
    imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VK_CHECK(vmaCreateImage(create_info->allocator, &imageCreateInfo, &allocCreateInfo, create_info->pTexArrayImg, create_info->pTexArrayAlloc, NULL));

    vkuTransitionImageLayout(*create_info->pTexArrayImg, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, create_info->mip_levels, create_info->layers, create_info->device, create_info->cmd_pool, create_info->graphics_queue);
    vkuCopyBufferToImage(create_info->device, create_info->cmd_pool, create_info->graphics_queue, stagingBuffer, *create_info->pTexArrayImg, create_info->width, create_info->height, create_info->layers);
    vkuGenerateMipmaps(create_info->physical_device, create_info->device, create_info->cmd_pool, create_info->graphics_queue, VK_FORMAT_R8G8B8A8_SRGB, *create_info->pTexArrayImg, create_info->width, create_info->height, create_info->mip_levels, create_info->layers);

    vmaDestroyBuffer(create_info->allocator, stagingBuffer, stagingBufferAllocation);
}

VkImageView vkuCreateTextureImageArrayView(VkDevice device, VkImage image, uint32_t mipLevels, uint32_t layerCount)
{
    VkImageView view = VK_NULL_HANDLE;

    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = image;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    viewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = mipLevels;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = layerCount;

    VK_CHECK(vkCreateImageView(device, &viewCreateInfo, NULL, &view));
    return view;
}

// VkSampler

typedef struct VkuSamplerCreateInfo
{
    VkFilter minFilter;
    VkFilter magFilter;
    VkSamplerAddressMode repeatMode;
    uint32_t mipmapLevels;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    float maxAnisotropy;
    VkBool32 enableCompare;
    VkCompareOp compareOp;
    VkBorderColor borderColor;
} VkuSamplerCreateInfo;

VkSampler vkuCreateSampler(VkuSamplerCreateInfo *create_info)
{
    VkSampler sampler = VK_NULL_HANDLE;

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = create_info->magFilter;
    samplerInfo.minFilter = create_info->minFilter;
    samplerInfo.addressModeU = create_info->repeatMode;
    samplerInfo.addressModeV = create_info->repeatMode;
    samplerInfo.addressModeW = create_info->repeatMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = create_info->maxAnisotropy;
    samplerInfo.borderColor = create_info->borderColor;

    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(create_info->physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = create_info->compareOp;
    samplerInfo.compareOp = create_info->compareOp ? create_info->compareOp : VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = (float)create_info->mipmapLevels;

    VK_CHECK(vkCreateSampler(create_info->device, &samplerInfo, NULL, &sampler));
    return sampler;
}

void vkuDestroySampler(VkSampler sampler, VkDevice device)
{
    vkDestroySampler(device, sampler, NULL);
}

// VkUniformBuffer

void vkuCreateUniformBuffers(VkuUniformBuffersCreateInfo *createInfo)
{
    (*createInfo->ppUniformBuffer) = (VkBuffer *)realloc(*createInfo->ppUniformBuffer, sizeof(VkBuffer) * createInfo->bufferCount);
    (*createInfo->ppUniformAllocations) = (VmaAllocation *)realloc(*createInfo->ppUniformAllocations, sizeof(VmaAllocation) * createInfo->bufferCount);
    (*createInfo->pppMappedUniformMemory) = (void **)realloc(*createInfo->pppMappedUniformMemory, sizeof(void *) * createInfo->bufferCount);

    VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = createInfo->bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    for (uint32_t i = 0; i < createInfo->bufferCount; i++)
    {
        VK_CHECK(vmaCreateBuffer(createInfo->allocator, &bufferInfo, &allocInfo, &(*createInfo->ppUniformBuffer)[i], &(*createInfo->ppUniformAllocations)[i], NULL));
        VK_CHECK(vmaMapMemory(createInfo->allocator, (*createInfo->ppUniformAllocations)[i], &(*createInfo->pppMappedUniformMemory)[i]));
    }
};

void vkuDestroyUniformBuffers(VmaAllocator allocator, VkBuffer *uniBuffer, VmaAllocation *uniAllocs, void **mappedUni, uint32_t bufferCount)
{
    for (uint32_t i = 0; i < bufferCount; i++)
    {
        if (mappedUni != NULL)
        {
            vmaUnmapMemory(allocator, uniAllocs[i]);
            (*mappedUni) = NULL;
        }

        vmaDestroyBuffer(allocator, uniBuffer[i], uniAllocs[i]);
    }

    free(uniBuffer);
    free(uniAllocs);
    free(mappedUni);
}

// DescriptorSet & Pool

VkDescriptorSetLayout vkuCreateDescriptorSetLayout(VkDevice device, VkuDescriptorSetAttribute *attribs, uint32_t attribCount)
{
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VkDescriptorSetLayoutBinding *bindings = (VkDescriptorSetLayoutBinding *)malloc(sizeof(VkDescriptorSetLayoutBinding) * attribCount);

    for (uint32_t i = 0; i < attribCount; ++i)
    {
        bindings[i].binding = i;
        bindings[i].descriptorCount = 1;
        bindings[i].pImmutableSamplers = NULL;
        bindings[i].stageFlags = attribs[i].shaderStage;

        if (attribs[i].type == VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER)
        {
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        if (attribs[i].type == VKU_DESCRIPTOR_SET_ATTRIB_UNIFORM_BUFFER)
        {
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }

        if (attribs[i].type == VKU_DESCRIPTOR_SET_ATTRIB_STORAGE_BUFFER)
        {
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        }
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = attribCount;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &set_layout) != VK_SUCCESS)
    {
        EXIT("Failed to create descriptor set layout!\n");
    }

    free(bindings);
    return set_layout;
}

void vkuDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout set_layout)
{
    vkDestroyDescriptorSetLayout(device, set_layout, NULL);
}

VkDescriptorPool vkuCreateDescriptorPool(VkDevice device, VkuDescriptorSetAttribute *attributes, uint32_t attribCount, uint32_t setCount)
{
    VkDescriptorPool pool = VK_NULL_HANDLE;

    VkDescriptorPoolSize poolSizes[attribCount];

    for (uint32_t i = 0; i < attribCount; i++)
    {
        if (attributes[i].type == VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER)
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        if (attributes[i].type == VKU_DESCRIPTOR_SET_ATTRIB_UNIFORM_BUFFER)
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        if (attributes[i].type == VKU_DESCRIPTOR_SET_ATTRIB_STORAGE_BUFFER)
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        poolSizes[i].descriptorCount = setCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = attribCount;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = setCount;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, NULL, &pool));
    return pool;
}

void vkuDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool)
{
    vkDestroyDescriptorPool(device, descriptorPool, NULL);
}

VkDescriptorSet *vkuCreateDescriptorSets(VkuDescriptorSetsCreateInfo *createInfo)
{
    VkDescriptorSetLayout layouts[createInfo->setCount];

    for (uint32_t i = 0; i < createInfo->setCount; i++)
        layouts[i] = createInfo->setLayout;

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = createInfo->pool;
    allocInfo.descriptorSetCount = createInfo->setCount;
    allocInfo.pSetLayouts = layouts;

    VkDescriptorSet *descriptor_sets = (VkDescriptorSet *)malloc(sizeof(VkDescriptorSet) * createInfo->setCount);
    if (descriptor_sets == NULL)
    {
        return NULL;
    }

    VK_CHECK(vkAllocateDescriptorSets(createInfo->device, &allocInfo, descriptor_sets));

    for (uint32_t i = 0; i < createInfo->setCount; i++)
    {
        VkWriteDescriptorSet descriptorWrites[createInfo->attribCount];
        memset(descriptorWrites, 0, sizeof(descriptorWrites));

        VkDescriptorImageInfo imageInfos[createInfo->attribCount];
        VkDescriptorBufferInfo bufferInfos[createInfo->attribCount];

        for (uint32_t j = 0; j < createInfo->attribCount; j++)
        {
            if (createInfo->attributes[j].type == VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER)
            {
                imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos[j].sampler = createInfo->attributes[j].sampler->sampler;
                if (createInfo->attributes[j].tex2D != NULL)
                    imageInfos[j].imageView = createInfo->attributes[j].tex2D->textureImageView;
                else if (createInfo->attributes[j].tex2DArray != NULL)
                    imageInfos[j].imageView = createInfo->attributes[j].tex2DArray->textureImageView;
                else
                    imageInfos[j].imageView = VK_NULL_HANDLE;

                descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[j].dstSet = descriptor_sets[i];
                descriptorWrites[j].dstBinding = j;
                descriptorWrites[j].dstArrayElement = 0;
                descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[j].descriptorCount = 1;
                descriptorWrites[j].pImageInfo = &imageInfos[j];
                descriptorWrites[j].pBufferInfo = NULL;
                descriptorWrites[j].pTexelBufferView = NULL;
            }

            if (createInfo->attributes[j].type == VKU_DESCRIPTOR_SET_ATTRIB_UNIFORM_BUFFER)
            {
                bufferInfos[j].buffer = createInfo->attributes[j].uniformBuffer->uniformBuffer[i];
                bufferInfos[j].offset = 0;
                bufferInfos[j].range = createInfo->attributes[j].uniformBuffer->bufferSize;

                descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[j].dstSet = descriptor_sets[i];
                descriptorWrites[j].dstBinding = j;
                descriptorWrites[j].dstArrayElement = 0;
                descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[j].descriptorCount = 1;
                descriptorWrites[j].pBufferInfo = &bufferInfos[j];
            }

            if (createInfo->attributes[j].type == VKU_DESCRIPTOR_SET_ATTRIB_STORAGE_BUFFER)
            {
                bufferInfos[j].buffer = createInfo->attributes[j].storageBuffer->buffer;
                bufferInfos[j].offset = 0; 
                bufferInfos[j].range = createInfo->attributes[j].storageBufferRange;

                descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[j].dstSet = descriptor_sets[i];
                descriptorWrites[j].dstBinding = j;
                descriptorWrites[j].dstArrayElement = 0;
                descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                descriptorWrites[j].descriptorCount = 1;
                descriptorWrites[j].pBufferInfo = &bufferInfos[j];
                descriptorWrites[j].pImageInfo = NULL;
                descriptorWrites[j].pTexelBufferView = NULL;
            }
        }

        vkUpdateDescriptorSets(createInfo->device, createInfo->attribCount, descriptorWrites, 0, NULL);
    }

    return descriptor_sets;
}

void vkuDestroyDescriptorSets(VkDescriptorSet *sets)
{
    free(sets);
}

// VkPipeline & layout

VkPipelineLayout vkuCreatePipelineLayout(VkDevice device, VkDescriptorSetLayout *setLayouts, uint32_t setLayoutCount)
{
    VkPipelineLayout layout = VK_NULL_HANDLE;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 128;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutInfo.pSetLayouts = setLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &layout));
    return layout;
}

void vkuDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout)
{
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
}

VkShaderModule vkuCreateShaderModule(const char *shaderCode, uint32_t codeLength, VkDevice device)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeLength;
    createInfo.pCode = (const uint32_t *)shaderCode;

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &createInfo, NULL, &shaderModule));

    return shaderModule;
}

VkVertexInputBindingDescription vkuGetVertexInputBindingDescription(VkuVertexLayout *layout)
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = (*layout).vertexSize;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

VkVertexInputAttributeDescription *vkuGetVertexAttributeDescriptions(VkuVertexLayout *layout)
{
    if ((*layout).attributeCount == 0)
        return VK_NULL_HANDLE;

    VkVertexInputAttributeDescription *attributeDescriptions = (VkVertexInputAttributeDescription *)malloc(sizeof(VkVertexInputAttributeDescription) * (*layout).attributeCount);

    for (uint32_t i = 0; i < (*layout).attributeCount; i++)
    {
        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = (*layout).attributes[i].format;
        attributeDescriptions[i].offset = (*layout).attributes[i].offset;
    }

    return attributeDescriptions;
}

VkPipeline vkuCreateGraphicsPipeline(VkuGraphicsPipelineCreateInfo *createInfo)
{
    VkShaderModule vertexShaderModule = vkuCreateShaderModule(createInfo->vertexShaderSpirv, createInfo->vertexShaderLength, createInfo->device);
    
    VkShaderModule fragmentShaderModule = NULL;
    if (createInfo->fragmentShaderSpirv != NULL && createInfo->fragmentShaderLength > 0) {
        fragmentShaderModule = vkuCreateShaderModule(createInfo->fragmentShaderSpirv, createInfo->fragmentShaderLength, createInfo->device);
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    uint32_t stageCount = 1;
    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo};
    if (fragmentShaderModule != NULL) {
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentShaderModule;
        fragShaderStageInfo.pName = "main";
        shaderStages[1] = fragShaderStageInfo;
        stageCount = 2;
    }

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    uint32_t dynamicStatesCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = (uint32_t)dynamicStatesCount;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription = vkuGetVertexInputBindingDescription(&createInfo->vertexInputLayout);
    VkVertexInputAttributeDescription *AttributeDescriptions = vkuGetVertexAttributeDescriptions(&createInfo->vertexInputLayout);

    vertexInputInfo.vertexBindingDescriptionCount = (createInfo->vertexInputLayout.attributeCount == 0) ? 0 : 1;
    vertexInputInfo.pVertexBindingDescriptions = (createInfo->vertexInputLayout.attributeCount == 0) ? NULL : &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = createInfo->vertexInputLayout.attributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = (createInfo->vertexInputLayout.attributeCount == 0) ? NULL : AttributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = createInfo->topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)createInfo->swapchainExtend.width;
    viewport.height = (float)createInfo->swapchainExtend.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = createInfo->swapchainExtend;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = createInfo->polygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = createInfo->cullMode;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = createInfo->enableDepthBias;
    rasterizer.depthBiasConstantFactor = createInfo->depthBiasConstantFactor;
    rasterizer.depthBiasSlopeFactor = createInfo->depthBiasSlopeFactor;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.rasterizationSamples = createInfo->msaaSamples;
    multisampling.minSampleShading = 0.2f;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = createInfo->depth_test_enable;
    depthStencil.depthWriteEnable = createInfo->depth_test_write;
    depthStencil.depthCompareOp = createInfo->depth_compare_mode;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    if (fragmentShaderModule == NULL) colorBlendAttachment.colorWriteMask = 0;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;

    if (fragmentShaderModule != NULL) {
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
    }

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = stageCount, // Updated dynamically
        .pStages = shaderStages, // Updated dynamically
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = createInfo->pipelineLayout,
        .renderPass = createInfo->renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VK_CHECK(vkCreateGraphicsPipelines(createInfo->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &graphicsPipeline));

    // Clean up shader modules
    vkDestroyShaderModule(createInfo->device, vertexShaderModule, NULL);
    if (fragmentShaderModule != NULL) {
        vkDestroyShaderModule(createInfo->device, fragmentShaderModule, NULL);
    }
    free(AttributeDescriptions);

    return graphicsPipeline;
}

void vkuDestroyVkPipeline(VkDevice device, VkPipeline pipeline)
{
    vkDestroyPipeline(device, pipeline, NULL);
}

VkPipeline vkuCreateComputeVkPipeline(VkuComputeVkPipelineCreateInfo *createInfo) {
    VkShaderModule computeShaderModule = vkuCreateShaderModule(createInfo->computeShaderSpirv, createInfo->computeShaderLength, createInfo->device);

    VkPipelineShaderStageCreateInfo shaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = computeShaderModule,
        .pName = "main"
    };

    VkComputePipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = shaderStageInfo,
        .layout = createInfo->pipelineLayout
    };

    VkPipeline pipeline;
    VK_CHECK(vkCreateComputePipelines(createInfo->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline));

    vkDestroyShaderModule(createInfo->device, computeShaderModule, NULL);

    return pipeline;
}

/** ######################################
 *  #                                    #
 *  #        VkUtils External            #
 *  #                                    #
 *  ######################################
 */

// External Function Predefines

// VkuQueue

VkuThreadSafeQueue vkuQueueCreate(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 1;
    }

    VkuThreadSafeQueue queue = (VkuThreadSafeQueue) malloc(sizeof(VkuThreadSafeQueue_T));
    if (!queue) return NULL;

    queue->data = (void **) malloc(initial_capacity * sizeof(void *));
    if (!queue->data) {
        free(queue);
        return NULL;
    }

    queue->capacity = initial_capacity;
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;

    pthread_mutex_init(&queue->lock, NULL);

    return queue;
}

void vkuQueueDestroy(VkuThreadSafeQueue queue) {
    if (!queue) return;

    pthread_mutex_destroy(&queue->lock);
    free(queue->data);
    free(queue);
}

static int vkuQueueResize(VkuThreadSafeQueue queue) {
    size_t new_capacity = queue->capacity * 2;
    void **new_data = (void **) malloc(new_capacity * sizeof(void *));
    if (!new_data) return -1;

    if (queue->front < queue->rear) {
        // Elements are in a contiguous block
        memcpy(new_data, &queue->data[queue->front], queue->size * sizeof(void *));
    } else {
        // Elements wrap around the end of the buffer
        size_t first_part_size = queue->capacity - queue->front;
        memcpy(new_data, &queue->data[queue->front], first_part_size * sizeof(void *));
        memcpy(&new_data[first_part_size], queue->data, queue->rear * sizeof(void *));
    }

    queue->front = 0;
    queue->rear = queue->size;
    queue->capacity = new_capacity;

    free(queue->data);
    queue->data = new_data;

    return 0;
}

int vkuQueueEnqueue(VkuThreadSafeQueue queue, void *item) {
    pthread_mutex_lock(&queue->lock);

    if (queue->size == queue->capacity) {
        if (vkuQueueResize(queue) != 0) {
            pthread_mutex_unlock(&queue->lock);
            return -1;
        }
    }

    queue->data[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    pthread_mutex_unlock(&queue->lock);
    return 0;
}

void *vkuQueueDequeue(VkuThreadSafeQueue queue) {
    pthread_mutex_lock(&queue->lock);

    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }

    void *item = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    pthread_mutex_unlock(&queue->lock);
    return item;
}

// VkuWindow

uint8_t *vkuLoadImage(const char *path, int *width, int *height, int *channels)
{
    stbi_set_flip_vertically_on_load(0);
    uint8_t *data = stbi_load(path, width, height, channels, STBI_rgb_alpha);
    if (data == NULL)
    {
        fprintf(stderr, "Failed to load image from path: %s\n", path);
        return NULL;
    }

    return data;
}

void framebuffer_size_callback(void *userdata, SDL_Event *event) {
    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        VkuWindow_T *vku_window = (VkuWindow_T *)userdata;
        if (vku_window)
            vku_window->window_resized = true;
    }
}

static bool SDLCALL WindowEventFilter(void* userdata, SDL_Event* event) {
    VkuWindow vkuWindow = (VkuWindow) userdata;

    if (event->type == SDL_EVENT_WINDOW_MINIMIZED) {
        vkuWindow->minimized = true;
    } else if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        vkuWindow->window_resized = true;
    }

    return true;
}

VkuWindow vkuCreateWindow(VkuWindowCreateInfo *createInfo) {
    VkuWindow_T *window = (VkuWindow_T *)calloc(1, sizeof(VkuWindow_T));

    window->sdlWindow = SDL_CreateWindow(createInfo->title, createInfo->width, createInfo->height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    if (!window->sdlWindow) {
        free(window);
        return NULL;
    }

    SDL_GetWindowSizeInPixels(window->sdlWindow, &window->windowedWidth, &window->windowedHeight);
    SDL_GetWindowPosition(window->sdlWindow, &window->windowedX, &window->windowedY);

    if (createInfo->window_icon_path != NULL) {
        int w, h, channels;
        void *pixels = vkuLoadImage(createInfo->window_icon_path, &w, &h, &channels); // Your custom loader
        if (pixels) {
            SDL_Surface *icon = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, 4*w);
            SDL_SetWindowIcon(window->sdlWindow, icon);
            SDL_DestroySurface(icon);
            stbi_image_free(pixels);
        }
    }

    SDL_AddEventWatch(WindowEventFilter, window);

    return window;
}

void vkuWindowToggleFullscreen(VkuWindow window) {
    if (!window->fullscreen) {
        SDL_GetWindowPosition(window->sdlWindow, &window->windowedX, &window->windowedY);
        SDL_GetWindowSizeInPixels(window->sdlWindow, &window->windowedWidth, &window->windowedHeight);
        SDL_SetWindowFullscreen(window->sdlWindow, SDL_WINDOW_FULLSCREEN);
        window->fullscreen = true;
    } else {
        SDL_SetWindowFullscreen(window->sdlWindow, 0);
        SDL_SetWindowPosition(window->sdlWindow, window->windowedX, window->windowedY);
        SDL_SetWindowSize(window->sdlWindow, window->windowedWidth, window->windowedHeight);
        window->fullscreen = false;
    }
}

void vkuWindowToggleInputMode(VkuWindow window) {
    SDL_Window *win = window->sdlWindow;
    int isRelative = SDL_GetWindowRelativeMouseMode(window->sdlWindow);
    SDL_SetWindowRelativeMouseMode(window->sdlWindow, !isRelative);
}

float vkuWindowGetAspect(VkuWindow window) {
    int width, height;
    SDL_GetWindowSize(window->sdlWindow, &width, &height);
    return (float)width / (float)height;
}

void vkuDestroyWindow(VkuWindow window) {
    if (window) {
        SDL_DestroyWindow(window->sdlWindow);
        free(window);
    }
}

// VkuMemoryManager

VkuMemoryManager vkuCreateMemoryManager(VkuMemoryManagerCreateInfo *createInfo)
{
    VkuMemoryManager_T *manager = (VkuMemoryManager_T *)calloc(1, sizeof(VkuMemoryManager_T));
    manager->device = createInfo->device;
    manager->transferQueue = createInfo->transferQueue;
    manager->allocator = vkuCreateVmaAllocator(createInfo->instance, createInfo->physicalDevice, createInfo->device);
    manager->transferCmdPool = vkuCreateCmdPool(createInfo->physicalDevice, createInfo->device, (createInfo->transferQueue != VK_NULL_HANDLE) ? VKU_CMD_POOL_TYPE_TRANSFER : VKU_CMD_POOL_TYPE_GRAPHICS);
    manager->fences = NULL;
    manager->fenceCount = 0;
    manager->destructionQueue = vkuQueueCreate(128);

    return manager;
}

void vkuDestroyMemoryManager(VkuMemoryManager memoryManager)
{
    vkuQueueDestroy(memoryManager->destructionQueue);
    vkuDestroyVmaAllocator(memoryManager->allocator);
    vkuDestroyCommandPool(memoryManager->device, memoryManager->transferCmdPool);
    free(memoryManager);
}

VkDeviceSize vkuMemoryMamgerGetAllocatedMemorySize(VkuMemoryManager manager)
{
    if (!manager)
    {
        return 0;
    }

    VmaTotalStatistics stats;
    vmaCalculateStatistics(manager->allocator, &stats);

    return stats.total.statistics.blockBytes;
}

VkuBuffer vkuCreateBuffer(VkuMemoryManager manager, VkDeviceSize size, VkuBufferUsage usage)
{
    VkuBuffer_T *buffer = (VkuBuffer_T *)calloc(1, sizeof(VkuBuffer_T));
    buffer->size = size;
    atomic_init(&buffer->queuedForDestruction, false);

    VkBufferCreateInfo bufferInfo = {};
    VmaAllocationCreateInfo allocInfo = {};

    if ((usage & VKU_BUFFER_USAGE_CPU_TO_GPU) == VKU_BUFFER_USAGE_CPU_TO_GPU)
    {
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    else if ((usage & VKU_BUFFER_USAGE_GPU_ONLY) == VKU_BUFFER_USAGE_GPU_ONLY)
    {
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        if ((usage & VKU_BUFFER_USAGE_COMPUTE) == VKU_BUFFER_USAGE_COMPUTE)
            bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    VK_CHECK(vmaCreateBuffer(manager->allocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, NULL));
    return buffer;
};

void vkuDestroyBuffer(VkuBuffer buffer, VkuMemoryManager manager, VkBool32 syncronize)
{
    if (buffer == NULL)
        return;

    if (syncronize)
    {
        if (manager->fenceCount >= 0)
        {
            vkWaitForFences(manager->device, manager->fenceCount, manager->fences, VK_TRUE, UINT64_MAX);
        }
        else
        {
            vkDeviceWaitIdle(manager->device);
        }
    }
    
    vmaDestroyBuffer(manager->allocator, buffer->buffer, buffer->allocation);
    free(buffer);
}

void vkuEnqueueBufferDestruction(VkuMemoryManager manager, VkuBuffer buffer)
{
    if (!vku_atomic_load(buffer->queuedForDestruction)) {
        vku_atomic_store(buffer->queuedForDestruction, true);
        vkuQueueEnqueue(manager->destructionQueue, (void*) buffer);
    }
}

void vkuDestroyBuffersInDestructionQueue(VkuMemoryManager manager, VkuPresenter syncPresenter) 
{
    if (syncPresenter != VK_NULL_HANDLE)
        vkWaitForFences(manager->device, manager->fenceCount, manager->fences, VK_TRUE, UINT64_MAX);
    else
        vkDeviceWaitIdle(manager->device);

    VkuBuffer buffer;
    while((buffer = (VkuBuffer) vkuQueueDequeue(manager->destructionQueue)) != NULL)
    {
        vmaDestroyBuffer(manager->allocator, buffer->buffer, buffer->allocation);
        free(buffer);
    }
}

void vkuSetBufferData(VkuMemoryManager manager, VkuBuffer buffer, void *data, size_t size)
{
    void *mapped_data = NULL;
    vmaMapMemory(manager->allocator, buffer->allocation, &mapped_data);
    memcpy(mapped_data, data, size);
    vmaUnmapMemory(manager->allocator, buffer->allocation);
}

void * vkuMapBuffer(VkuMemoryManager manager, VkuBuffer buffer)
{
    void *mapped_data = NULL;
    vmaMapMemory(manager->allocator, buffer->allocation, &mapped_data);
    return mapped_data;
}

void vkuUnmapBuffer(VkuMemoryManager manager, VkuBuffer buffer)
{
    vmaUnmapMemory(manager->allocator, buffer->allocation);
}

void vkuCopyBuffer(VkuMemoryManager manager, VkuBuffer *srcBuffer, VkuBuffer *dstBuffer, VkDeviceSize *size, uint32_t count)
{
    uint32_t greater_than_null = 0;
    for (uint32_t i = 0; i < count; i++)
        if (size[i] > 0)
            greater_than_null++;

    if (greater_than_null <= 0)
        return;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = manager->transferCmdPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(manager->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    for (uint32_t i = 0; i < count; i++)
    {
        if (size[i] <= 0)
            continue;

        VkBufferCopy copyRegion = {};
        copyRegion.size = size[i];

        vkCmdCopyBuffer(commandBuffer, srcBuffer[i]->buffer, dstBuffer[i]->buffer, 1, &copyRegion);
    }

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(manager->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(manager->transferQueue);

    vkFreeCommandBuffers(manager->device, manager->transferCmdPool, 1, &commandBuffer);
}

// VkuContext

VkuContext vkuCreateContext(VkuContextCreateInfo *createInfo)
{

    VkuContext_T *context = (VkuContext_T *)calloc(1, sizeof(VkuContext_T));
    context->validation = createInfo->enableValidation;
    context->usageFlags = createInfo->usage;

    VkuVkInstanceCreateInfo instanceCreateInfo = {
        .enableValidation = createInfo->enableValidation,
        .applicationName = createInfo->applicationName,
        .applicationVersion = createInfo->applicationVersion,
    };

    context->instance = vkuCreateVkInstance(&instanceCreateInfo);

    context->debugMessenger = vkuCreateVkDebugMessenger(context->instance, createInfo->enableValidation);

    if ((createInfo->usage & VKU_CONTEXT_USAGE_PRESENTATION) != VKU_CONTEXT_USAGE_PRESENTATION)
    {
        context->physicalDevice = vkuGetOptimalPhysicalDevice(context->instance, VK_NULL_HANDLE);

        VkuVkDeviceCreateInfo deviceCreateInfo = {
            .instance = context->instance,
            .surface = VK_NULL_HANDLE,
            .physical_device = context->physicalDevice,
            .enable_validation = createInfo->enableValidation,
            .pGraphicsQueue = &context->graphicsQueue,
            .pPresentQueue = &context->presentQueue,
            .pTransferQueue = &context->transferQueue,
            .pComputeQueue = &context->computeQueue,
        };

        context->device = vkuCreateVkDevice(&deviceCreateInfo);

        VkuMemoryManagerCreateInfo memoryManagerCreateInfo = {
            .device = context->device,
            .transferQueue = context->transferQueue,
            .physicalDevice = context->physicalDevice,
            .instance = context->instance,
        };

        context->memoryManager = vkuCreateMemoryManager(&memoryManagerCreateInfo);

        context->graphicsCmdPool = vkuCreateCmdPool(context->physicalDevice, context->device, VKU_CMD_POOL_TYPE_GRAPHICS);
        context->computeCmdPool = vkuCreateCmdPool(context->physicalDevice, context->device, VKU_CMD_POOL_TYPE_COMPUTE);
    }
    else
    {
        context->physicalDevice = VK_NULL_HANDLE;
        context->device = VK_NULL_HANDLE;
        context->memoryManager = NULL;
    }

    return context;
}

void vkuDestroyContext(VkuContext context)
{
    if (context->device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(context->device);
        vkuDestroyCommandPool(context->device, context->computeCmdPool);
        vkuDestroyCommandPool(context->device, context->graphicsCmdPool);
        vkuDestroyMemoryManager(context->memoryManager);
        vkuDestroyVkDevice(context->device);
        vkuDestroyVkDebugMessenger(context->instance, context->debugMessenger);
        vkuDestroyVkInstance(context->instance);
    }

    free(context);
}

VkSampleCountFlagBits vkuContextGetMaxSampleCount(VkuContext context)
{
    return vkuGetMaxUsableSampleCount(context->physicalDevice);
}

void vkuContextPhysicalDeviceWithSurfaceSupport(VkuContext context, VkSurfaceKHR surface)
{
    if ((context->usageFlags & VKU_CONTEXT_USAGE_PRESENTATION) != VKU_CONTEXT_USAGE_PRESENTATION)
        EXIT("VkuError: Please set VKU_CONTEXT_USAGE_PRESENTATION flag during vkuContextCreation when using a vkuPresenter!\n");

    if ((context->usageFlags & VKU_CONTEXT_USAGE_OFFSCREEN) == VKU_CONTEXT_USAGE_OFFSCREEN)
        printf("VkuPerformanceWarning: When VKU_CONTEXT_USAGE_PRESENTATION | VKU_CONTEXT_USAGE_BASIC is set, Objects have to be recreated internally during vkuPresenter creation!\n");

    VkPhysicalDevice supportedPhysicalDevice = vkuGetOptimalPhysicalDevice(context->instance, surface);

    if (context->memoryManager != NULL)
    {
        vkuDestroyMemoryManager(context->memoryManager);
    }

    if (context->graphicsCmdPool != VK_NULL_HANDLE)
    {
        vkuDestroyCommandPool(context->device, context->graphicsCmdPool);
    }

    if (context->computeCmdPool != VK_NULL_HANDLE)
    {
        vkuDestroyCommandPool(context->device, context->computeCmdPool);
    }

    if (context->device != VK_NULL_HANDLE)
    {
        vkuDestroyVkDevice(context->device);
    }

    context->physicalDevice = supportedPhysicalDevice;

    VkuVkDeviceCreateInfo deviceCreateInfo = {
        .instance = context->instance,
        .surface = surface,
        .physical_device = context->physicalDevice,
        .enable_validation = context->validation,
        .pGraphicsQueue = &context->graphicsQueue,
        .pPresentQueue = &context->presentQueue,
        .pTransferQueue = &context->transferQueue,
        .pComputeQueue = &context->computeQueue,
    };

    context->device = vkuCreateVkDevice(&deviceCreateInfo);

    VkuMemoryManagerCreateInfo memoryManagerCreateInfo = {
        .device = context->device,
        .transferQueue = context->transferQueue,
        .physicalDevice = context->physicalDevice,
        .instance = context->instance,
    };

    context->memoryManager = vkuCreateMemoryManager(&memoryManagerCreateInfo);

    context->graphicsCmdPool = vkuCreateCmdPool(context->physicalDevice, context->device, VKU_CMD_POOL_TYPE_GRAPHICS);
    context->computeCmdPool = vkuCreateCmdPool(context->physicalDevice, context->device, VKU_CMD_POOL_TYPE_COMPUTE);
}

VkuMemoryManager vkuContextGetMemoryManager(VkuContext context)
{
    return context->memoryManager;
}

// VkuRenderResourceManager

VkuRenderResourceManager vkuCreateRenderResourceManager(VkuPresenter presenter)
{
    VkuRenderResourceManager_T *mgr = (VkuRenderResourceManager_T *)calloc(1, sizeof(VkuRenderResourceManager_T));
    mgr->presenter = presenter;
    mgr->maxSamplesUint32 = find_bit_position(vkuGetMaxUsableSampleCount(presenter->context->physicalDevice)) + 1;

    mgr->colorResources = (VkuColorResource *)calloc(1, sizeof(VkuColorResource) * mgr->maxSamplesUint32);
    mgr->depthResources = (VkuDepthResource *)calloc(1, sizeof(VkuDepthResource) * mgr->maxSamplesUint32);
    mgr->sampleFlags = (VkSampleCountFlagBits *)calloc(1, sizeof(VkSampleCountFlagBits) * mgr->maxSamplesUint32);

    return mgr;
}

VkuColorResource vkuRenderResourceManagerGetColorResource(VkuRenderResourceManager resourceManager, VkSampleCountFlagBits sampleCount)
{
    uint32_t index = find_bit_position(sampleCount);

    if (resourceManager->colorResources[index] == NULL)
    {
        VkuColorResourcesCreateInfo colorResourcesCreateInfo = {
            .format = resourceManager->presenter->swapchainFormat,
            .allocator = resourceManager->presenter->context->memoryManager->allocator,
            .extend = resourceManager->presenter->swapchainExtend,
            .msaa_samples = sampleCount,
            .device = resourceManager->presenter->context->device,
        };

        resourceManager->colorResources[index] = vkuCreateColorResources(&colorResourcesCreateInfo);
        resourceManager->sampleFlags[index] = sampleCount;
    }

    return resourceManager->colorResources[index];
}

VkuDepthResource vkuRenderResourceManagerGetDepthResource(VkuRenderResourceManager resourceManager, VkSampleCountFlagBits sampleCount)
{
    uint32_t index = find_bit_position(sampleCount);

    if (resourceManager->depthResources[index] == NULL)
    {
        VkuDepthResourcesCreateInfo depthResourcesCreateInfo = {
            .physical_device = resourceManager->presenter->context->physicalDevice,
            .device = resourceManager->presenter->context->device,
            .allocator = resourceManager->presenter->context->memoryManager->allocator,
            .extend = resourceManager->presenter->swapchainExtend,
            .msaa_samples = sampleCount,
            .layerCount = 1
        };

        resourceManager->depthResources[index] = vkuCreateDepthResources(&depthResourcesCreateInfo);
        resourceManager->sampleFlags[index] = sampleCount;
    }

    return resourceManager->depthResources[index];
}

void vkuRenderResourceManagerUpdate(VkuRenderResourceManager resourceManager)
{
    for (uint32_t i = 0; i < resourceManager->maxSamplesUint32; i++)
    {
        if (resourceManager->colorResources[i] != NULL)
        {
            vkuDestroyColorResources(resourceManager->presenter->context->memoryManager->allocator, resourceManager->presenter->context->device, resourceManager->colorResources[i]);

            VkuColorResourcesCreateInfo colorResourcesCreateInfo = {
                .format = resourceManager->presenter->swapchainFormat,
                .allocator = resourceManager->presenter->context->memoryManager->allocator,
                .extend = resourceManager->presenter->swapchainExtend,
                .msaa_samples = resourceManager->sampleFlags[i],
                .device = resourceManager->presenter->context->device,
            };

            resourceManager->colorResources[i] = vkuCreateColorResources(&colorResourcesCreateInfo);
        }

        if (resourceManager->depthResources[i] != NULL)
        {
            vkuDestroyDepthResources(resourceManager->presenter->context->memoryManager->allocator, resourceManager->presenter->context->device, resourceManager->depthResources[i]);

            VkuDepthResourcesCreateInfo depthResourcesCreateInfo = {
                .physical_device = resourceManager->presenter->context->physicalDevice,
                .device = resourceManager->presenter->context->device,
                .allocator = resourceManager->presenter->context->memoryManager->allocator,
                .extend = resourceManager->presenter->swapchainExtend,
                .msaa_samples = resourceManager->sampleFlags[i],
                .layerCount = 1
            };

            resourceManager->depthResources[i] = vkuCreateDepthResources(&depthResourcesCreateInfo);
        }
    }
}

void vkuDestroyRenderResourceManager(VkuRenderResourceManager resourceManager)
{
    for (uint32_t i = 0; i < resourceManager->maxSamplesUint32; i++)
    {
        if (resourceManager->colorResources[i] != NULL)
            vkuDestroyColorResources(resourceManager->presenter->context->memoryManager->allocator, resourceManager->presenter->context->device, resourceManager->colorResources[i]);

        if (resourceManager->depthResources[i] != NULL)
            vkuDestroyDepthResources(resourceManager->presenter->context->memoryManager->allocator, resourceManager->presenter->context->device, resourceManager->depthResources[i]);
    }

    free(resourceManager->colorResources);
    free(resourceManager->depthResources);
    free(resourceManager->sampleFlags);
    free(resourceManager);
}

// VkuPresenter

VkuPresenter vkuCreatePresenter(VkuPresenterCreateInfo *createInfo)
{
    VkuPresenter_T *presenter = (VkuPresenter_T *)calloc(1, sizeof(VkuPresenter_T));
    presenter->context = createInfo->context;
    presenter->framesInFlight = createInfo->framesInFlight;

    VkuWindowCreateInfo windowCreateInfo = {
        .width = createInfo->width,
        .height = createInfo->height,
        .centered = true,
        .title = createInfo->windowTitle,
        .window_icon_path = createInfo->windowIconPath};

    presenter->window = vkuCreateWindow(&windowCreateInfo);

    presenter->surface = vkuCreateSurface(createInfo->context->instance, presenter->window->sdlWindow);
    vkuContextPhysicalDeviceWithSurfaceSupport(createInfo->context, presenter->surface);

    presenter->presentMode = createInfo->presentMode;

    VkuVkSwapchainKHRCreateInfo swapchainCreateInfo = {
        .physicalDevice = createInfo->context->physicalDevice,
        .device = createInfo->context->device,
        .surface = presenter->surface,
        .sdlWindow = presenter->window->sdlWindow,
        .presentMode = presenter->presentMode,
        .pSwapchainImageCount = &presenter->swapchainImageCount,
        .ppSwapchainImages = &presenter->swapchainImages,
        .pSwapchainFormat = &presenter->swapchainFormat,
        .pSwapchainExtent = &presenter->swapchainExtend,
    };

    presenter->swapchain = vkuCreateVkSwapchainKHR(&swapchainCreateInfo);

    presenter->swapchainImageViews = vkuCreateSwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImages, presenter->swapchainFormat, createInfo->context->device);

    presenter->cmdBuffer = vkuCreateCommandBuffer(presenter->context->device, presenter->framesInFlight, presenter->context->graphicsCmdPool);

    VkuSyncObjectsCreateInfo syncCreateInfo = {
        .ppImageAvailableSemaphores = &presenter->imageAvailableSemaphores,
        .ppRenderFinishedSemaphores = &presenter->renderFinishedSemaphores,
        .ppInFlightFences = &presenter->inFlightFences,
        .framesInFlight = presenter->framesInFlight,
        .device = presenter->context->device,
    };

    vkuCreateSyncObjects(&syncCreateInfo);

    presenter->resourceManager = vkuCreateRenderResourceManager(presenter);
    presenter->renderStageManager = vkuCreateObjectManager(sizeof(VkuRenderStage));

    presenter->context->memoryManager->fences = presenter->inFlightFences;
    presenter->context->memoryManager->fenceCount = presenter->framesInFlight;

    return presenter;
}

void vkuRenderStageUpdate(VkuRenderStage renderStage);

void vkuPresenterRecreate(VkuPresenter presenter)
{
    int width = 0, height = 0;
    SDL_GetWindowSizeInPixels(presenter->window->sdlWindow, &width, &height);

    SDL_Event event;
    while (presenter->window->minimized || width == 0 || height == 0) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_EVENT_WINDOW_RESTORED) {
                presenter->window->minimized = false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                SDL_GetWindowSizeInPixels(presenter->window->sdlWindow, &width, &height);
            }
        }
    }

    // Cleanup
    vkDeviceWaitIdle(presenter->context->device);

    vkuDestroySwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImageViews, presenter->context->device);
    vkuDestroyVkSwapchainKHR(presenter->swapchain, presenter->context->device, presenter->swapchainImages);

    // Creation
    VkuVkSwapchainKHRCreateInfo swapchainCreateInfo = {
        .physicalDevice = presenter->context->physicalDevice,
        .device = presenter->context->device,
        .surface = presenter->surface,
        .sdlWindow = presenter->window->sdlWindow,
        .presentMode = presenter->presentMode,
        .pSwapchainImageCount = &presenter->swapchainImageCount,
        .ppSwapchainImages = &presenter->swapchainImages,
        .pSwapchainFormat = &presenter->swapchainFormat,
        .pSwapchainExtent = &presenter->swapchainExtend,
    };

    presenter->swapchain = vkuCreateVkSwapchainKHR(&swapchainCreateInfo);
    presenter->swapchainImageViews = vkuCreateSwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImages, presenter->swapchainFormat, presenter->context->device);

    vkuRenderResourceManagerUpdate(presenter->resourceManager);

    for (uint32_t i = 0; i < presenter->renderStageManager->elemCnt; i++)
    {
        vkuRenderStageUpdate((VkuRenderStage)presenter->renderStageManager->elements[i]);
    }
}

void vkuPresenterSetPresentMode(VkuPresenter presenter, VkPresentModeKHR presentMode)
{
    vkDeviceWaitIdle(presenter->context->device);

    presenter->presentMode = presentMode;

    vkuDestroySwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImageViews, presenter->context->device);
    vkuDestroyVkSwapchainKHR(presenter->swapchain, presenter->context->device, presenter->swapchainImages);

    VkuVkSwapchainKHRCreateInfo swapchainCreateInfo = {
        .physicalDevice = presenter->context->physicalDevice,
        .device = presenter->context->device,
        .surface = presenter->surface,
        .sdlWindow = presenter->window->sdlWindow,
        .presentMode = presenter->presentMode,
        .pSwapchainImageCount = &presenter->swapchainImageCount,
        .ppSwapchainImages = &presenter->swapchainImages,
        .pSwapchainFormat = &presenter->swapchainFormat,
        .pSwapchainExtent = &presenter->swapchainExtend,
    };

    presenter->swapchain = vkuCreateVkSwapchainKHR(&swapchainCreateInfo);
    presenter->swapchainImageViews = vkuCreateSwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImages, presenter->swapchainFormat, presenter->context->device);

    for (uint32_t i = 0; i < presenter->renderStageManager->elemCnt; i++)
    {
        vkuRenderStageUpdate((VkuRenderStage)presenter->renderStageManager->elements[i]);
    }
}

VkuWindow vkuPresenterGetWindow(VkuPresenter presenter)
{
    return presenter->window;
}

uint32_t vkuPresenterGetFramesInFlight(VkuPresenter presenter)
{
    return presenter->framesInFlight;
}

void vkuDestroyPresenter(VkuPresenter presenter)
{
    vkDeviceWaitIdle(presenter->context->device);

    vkuDestroyObjectManager(presenter->renderStageManager);
    vkuDestroyRenderResourceManager(presenter->resourceManager);
    vkuDestroySyncObjects(presenter->context->device, presenter->framesInFlight, presenter->imageAvailableSemaphores, presenter->renderFinishedSemaphores, presenter->inFlightFences);
    vkuDestroyCmdBuffer(presenter->cmdBuffer);
    vkuDestroySwapchainImageViews(presenter->swapchainImageCount, presenter->swapchainImageViews, presenter->context->device);
    vkuDestroyVkSwapchainKHR(presenter->swapchain, presenter->context->device, presenter->swapchainImages);
    vkuDestroySurface(presenter->surface, presenter->context->instance);
    vkuDestroyWindow(presenter->window);
    free(presenter);
}

uint8_t *vkuPresenterRetrieveSwapchainImage(VkuPresenter presenter, uint32_t *outWidth, uint32_t *outHeight) {
    if (presenter->swapchainExtend.width == 0 || presenter->swapchainExtend.height == 0) {
        EXIT("vkuPresenterRetrieveSwapchainImage: Invalid swapchain extent!\n");
        return NULL;
    }

    vkQueueWaitIdle(presenter->context->graphicsQueue);

    VkImage dstImage;
    VmaAllocation dstAlloc;
    VmaAllocationInfo allocInfo;

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_B8G8R8A8_SRGB,
        .extent = {presenter->swapchainExtend.width, presenter->swapchainExtend.height, 1},
        .mipLevels = 1, .arrayLayers = 1, .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_LINEAR,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocCreateInfo = {
        .usage = VMA_MEMORY_USAGE_AUTO,
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
    };

    if (vmaCreateImage(presenter->context->memoryManager->allocator, &imageInfo, &allocCreateInfo, &dstImage, &dstAlloc, &allocInfo) != VK_SUCCESS) {
        EXIT("Failed to create destination image\n");
        return NULL;
    }

    VkCommandBuffer cmdBuffer;
    VkCommandBufferAllocateInfo cmdAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = presenter->context->graphicsCmdPool,
        .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(presenter->context->device, &cmdAllocInfo, &cmdBuffer) != VK_SUCCESS) {
        vmaDestroyImage(presenter->context->memoryManager->allocator, dstImage, dstAlloc);
        EXIT("Failed to allocate command buffer!\n");
        return NULL;
    }

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
        vkFreeCommandBuffers(presenter->context->device, presenter->context->graphicsCmdPool, 1, &cmdBuffer);
        vmaDestroyImage(presenter->context->memoryManager->allocator, dstImage, dstAlloc);
        EXIT("Failed to begin command buffer\n");
        return NULL;
    }

    VkImageMemoryBarrier barriers[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcAccessMask = 0, .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .image = presenter->swapchainImages[presenter->activeFrame],
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
        },
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcAccessMask = 0, .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .image = dstImage,
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
        }
    };

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 2, barriers);

    VkImageCopy region = {
        .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .extent = {presenter->swapchainExtend.width, presenter->swapchainExtend.height, 1}
    };

    vkCmdCopyImage(cmdBuffer, presenter->swapchainImages[presenter->activeFrame], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkImageMemoryBarrier presentBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT, .dstAccessMask = 0,
        .image = presenter->swapchainImages[presenter->activeFrame],
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    };

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &presentBarrier);
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &cmdBuffer};
    if (vkQueueSubmit(presenter->context->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        vkFreeCommandBuffers(presenter->context->device, presenter->context->graphicsCmdPool, 1, &cmdBuffer);
        vmaDestroyImage(presenter->context->memoryManager->allocator, dstImage, dstAlloc);
        EXIT("Failed to submit command buffer\n");
        return NULL;
    }

    vkQueueWaitIdle(presenter->context->graphicsQueue);
    vkFreeCommandBuffers(presenter->context->device, presenter->context->graphicsCmdPool, 1, &cmdBuffer);

    *outWidth = presenter->swapchainExtend.width;
    *outHeight = presenter->swapchainExtend.height;

    size_t imageSize = *outWidth * *outHeight * 4;
    uint8_t *imageData = malloc(imageSize);
    if (!imageData) {
        vmaDestroyImage(presenter->context->memoryManager->allocator, dstImage, dstAlloc);
        EXIT("Failed to allocate image data\n");
        return NULL;
    }

    VkImageSubresource subresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .arrayLayer = 0};
    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(presenter->context->device, dstImage, &subresource, &layout);

    uint8_t *src = allocInfo.pMappedData;
    for (uint32_t y = 0; y < *outHeight; ++y)
        memcpy(imageData + y * *outWidth * 4, src + y * layout.rowPitch, *outWidth * 4);

    vmaDestroyImage(presenter->context->memoryManager->allocator, dstImage, dstAlloc);
    return imageData;
}

bool vkuWriteImage(const uint8_t *pixelData, uint32_t width, uint32_t height, uint32_t channels, const char *filename, bool forceOpaque) {
    if (!pixelData || width == 0 || height == 0 || channels != 4 || !filename) {
        fprintf(stderr, "vkuWriteImage: Invalid parameters (requires 4 channels)\n");
        return false;
    }

    size_t imageSize = width * height * 4;
    uint8_t *convertedData = (uint8_t *)malloc(imageSize);
    if (!convertedData) {
        fprintf(stderr, "vkuWriteImage: Failed to allocate memory\n");
        return false;
    }

    for (size_t i = 0; i < width * height; ++i) {
        size_t idx = i * 4;
        convertedData[idx + 0] = pixelData[idx + 2]; // R
        convertedData[idx + 1] = pixelData[idx + 1]; // G
        convertedData[idx + 2] = pixelData[idx + 0]; // B
        convertedData[idx + 3] = forceOpaque ? 255 : pixelData[idx + 3]; // A
    }

    int stride = width * 4;
    int success = stbi_write_png(filename, width, height, 4, convertedData, stride);

    free(convertedData);

    if (!success) {
        EXIT("vkuWriteImage: Failed to write image.\n");
        return false;
    }

    return true;
}

// VkuRenderStage

VkuRenderStage vkuCreateRenderStage(VkuRenderStageCreateInfo *createInfo)
{
    VkuRenderStage_T *renderStage = (VkuRenderStage_T *)calloc(1, sizeof(VkuRenderStage_T));
    renderStage->options = createInfo->options;
    renderStage->presenter = createInfo->presenter;
    renderStage->context = createInfo->presenter->context;
    renderStage->sampleCount = createInfo->msaaSamples;
    renderStage->enableDepthTesting = createInfo->enableDepthTesting;
    renderStage->outputCount = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? renderStage->presenter->swapchainImageCount : 1;
    renderStage->staticRenderStage = VK_FALSE;
    renderStage->staticDepthArrayCount = 1;

    if ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER)
    {
        if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE || (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE)
            EXIT("VkuError: A RenderStage cant have VKU_RENDER_OPTION_DEPTH_IMAGE and / or VKU_RENDER_OPTION_COLOR_IMAGE enabled along with VKU_RENDER_OPTION_PRESENTER!\n");

        renderStage->pTargetColorImgViews = renderStage->presenter->swapchainImageViews;
    }
    else
    {
        if (!renderStage->enableDepthTesting && (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE)
            EXIT("VkuError: RenderStage is created with VKU_RENDER_OPTION_DEPTH_IMAGE option enabled, but enableDepthTesting is disabled!\n");

        renderStage->pTargetColorImages = (VkImage *)calloc(1, sizeof(VkImage));
        renderStage->pTargetColorImgViews = (VkImageView *)calloc(1, sizeof(VkImageView));
        renderStage->pTargetColorImgAllocs = (VmaAllocation *)calloc(1, sizeof(VmaAllocation));
        renderStage->pTargetDepthImages = (VkImage *)calloc(1, sizeof(VkImage));
        renderStage->pTargetDepthImgViews = (VkImageView *)calloc(1, sizeof(VkImageView));
        renderStage->pTargetDepthImgAllocs = (VmaAllocation *)calloc(1, sizeof(VmaAllocation));

        if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE)
        {
            VkuVkImageCreateInfo imageCreateInfo = {
                .allocator = renderStage->presenter->context->memoryManager->allocator,
                .width = renderStage->presenter->swapchainExtend.width,
                .height = renderStage->presenter->swapchainExtend.height,
                .mipLevels = 1,
                .format = VK_FORMAT_B8G8R8A8_SRGB,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .numSamples = VK_SAMPLE_COUNT_1_BIT,
                .pImage = &(renderStage->pTargetColorImages[0]),
                .pImageAlloc = &(renderStage->pTargetColorImgAllocs[0]),
                .pImageAllocInfo = NULL,
                .arrayLayers = 1
            };

            vkuCreateImage(&imageCreateInfo);

            renderStage->pTargetColorImgViews[0] = vkuCreateImageView(renderStage->pTargetColorImages[0], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, renderStage->presenter->context->device);
        }

        if ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE && renderStage->enableDepthTesting)
        {
            VkuVkImageCreateInfo imageCreateInfo = {
                .allocator = renderStage->presenter->context->memoryManager->allocator,
                .width = renderStage->presenter->swapchainExtend.width,
                .height = renderStage->presenter->swapchainExtend.height,
                .mipLevels = 1,
                .format = VK_FORMAT_D32_SFLOAT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .numSamples = VK_SAMPLE_COUNT_1_BIT,
                .pImage = &(renderStage->pTargetDepthImages[0]),
                .pImageAlloc = &(renderStage->pTargetDepthImgAllocs[0]),
                .pImageAllocInfo = NULL,
                .arrayLayers = 1
            };

            vkuCreateImage(&imageCreateInfo);

            renderStage->pTargetDepthImgViews[0] = vkuCreateImageView(renderStage->pTargetDepthImages[0], VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1, renderStage->presenter->context->device);
        }
    }

    if (renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT && ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE || (renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER))
    {
        renderStage->colorResource = vkuRenderResourceManagerGetColorResource(renderStage->presenter->resourceManager, renderStage->sampleCount);
    }

    if ((renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT || (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) != VKU_RENDER_OPTION_DEPTH_IMAGE) && renderStage->enableDepthTesting)
    {
        renderStage->depthResource = vkuRenderResourceManagerGetDepthResource(renderStage->presenter->resourceManager, renderStage->sampleCount);
    }

    VkuVkRenderPassCreateInfo renderPassCreateInfo = {
        .format = renderStage->presenter->swapchainFormat,
        .msaaSamples = renderStage->sampleCount,
        .physicalDevice = renderStage->presenter->context->physicalDevice,
        .device = renderStage->presenter->context->device,
        .enableDepthTest = renderStage->enableDepthTesting,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? true : ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? false : ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .presentationLayout = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER),
    };

    renderStage->renderPass = vkuCreateVkRenderPass(&renderPassCreateInfo);

    VkuVkFramebufferCreateInfo frameBufferCreateInfo = {
        .imageCount = renderStage->outputCount,
        .msaaSamples = renderStage->sampleCount,
        .renderStageColorImageView = (renderStage->colorResource) ? renderStage->colorResource->imageView : VK_NULL_HANDLE,
        .renderStageDepthImageView = (renderStage->depthResource) ? renderStage->depthResource->imageView : VK_NULL_HANDLE,
        .renderTargetColorImageViews = renderStage->pTargetColorImgViews,
        .renderTargetDepthImageViews = renderStage->pTargetDepthImgViews,
        .renderPass = renderStage->renderPass,
        .extend = renderStage->presenter->swapchainExtend,
        .device = renderStage->presenter->context->device,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? true : ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? false : ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .enableDepthTest = renderStage->enableDepthTesting,
        .layerCount = 1};

    renderStage->framebuffers = vkuCreateVkFramebuffer(&frameBufferCreateInfo);

    renderStage->pipelineManager = vkuCreateObjectManager(sizeof(VkuPipeline));
    renderStage->outputTextureManager = vkuCreateObjectManager(sizeof(VkuTexture2D));
    renderStage->descriptorSetManager = vkuCreateObjectManager(sizeof(VkuDescriptorSet));

    vkuObjectManagerAdd(renderStage->presenter->renderStageManager, (void *)renderStage);
    return renderStage;
}

VkuTexture2D vkuRenderStageGetDepthOutput(VkuRenderStage renderStage)
{
    if ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) != VKU_RENDER_OPTION_DEPTH_IMAGE)
        EXIT("VkuError: Depth Texture cant be retrieved: RenderStage has no depth output!\n");

    VkuTexture2D_T *tex = (VkuTexture2D_T *)calloc(1, sizeof(VkuTexture2D_T));
    tex->renderStage = renderStage;
    tex->renderStageDepthImage = VK_TRUE;
    tex->renderStageColorImage = VK_FALSE;
    tex->textureImage = VK_NULL_HANDLE;
    tex->textureImageAllocation = VK_NULL_HANDLE;
    tex->textureImageView = renderStage->pTargetDepthImgViews[0];
    tex->imageExtend = (renderStage->staticRenderStage == VK_FALSE) ? renderStage->presenter->swapchainExtend : renderStage->extend;

    vkuObjectManagerAdd(tex->renderStage->outputTextureManager, (void *)tex);

    return tex;
}

VkuTexture2D vkuRenderStageGetColorOutput(VkuRenderStage renderStage)
{
    if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) != VKU_RENDER_OPTION_COLOR_IMAGE)
        EXIT("VkuError: Color Texture cant be retrieved: RenderStage has no color output or renders directly into the swapchain!\n");

    VkuTexture2D_T *tex = (VkuTexture2D_T *)calloc(1, sizeof(VkuTexture2D_T));
    tex->renderStage = renderStage;
    tex->renderStageDepthImage = VK_FALSE;
    tex->renderStageColorImage = VK_TRUE;
    tex->textureImage = VK_NULL_HANDLE;
    tex->textureImageAllocation = VK_NULL_HANDLE;
    tex->textureImageView = renderStage->pTargetColorImgViews[0];
    tex->imageExtend = (renderStage->staticRenderStage == VK_FALSE) ? renderStage->presenter->swapchainExtend : renderStage->extend;

    vkuObjectManagerAdd(tex->renderStage->outputTextureManager, (void *)tex);

    return tex;
}

void vkuRenderStageOutputTextureUpdate(VkuTexture2D texture)
{
    if (texture->renderStageColorImage)
    {
        texture->textureImageView = texture->renderStage->pTargetColorImgViews[0];
    }
    else if (texture->renderStageDepthImage)
    {
        texture->textureImageView = texture->renderStage->pTargetDepthImgViews[0];
    }

    texture->imageExtend = (texture->renderStage->staticRenderStage == VK_FALSE) ? texture->renderStage->presenter->swapchainExtend : texture->renderStage->extend;
}

void vkuPipelineUpdate(VkuPipeline pipeline);
void vkuDescriptorSetUpdate(VkuDescriptorSet descriptorSet);

void vkuRenderStageUpdate(VkuRenderStage renderStage)
{
    vkDeviceWaitIdle(renderStage->context->device);
    vkuDestroyFramebuffer(renderStage->presenter->context->device, renderStage->framebuffers, renderStage->outputCount);
    vkuDestroyVkRenderPass(renderStage->presenter->context->device, renderStage->renderPass);

    if ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) != VKU_RENDER_OPTION_PRESENTER)
    {
        if (renderStage->pTargetColorImgViews[0] != NULL)
        {
            vkuDestroyImage(renderStage->pTargetColorImages[0], renderStage->pTargetColorImgAllocs[0], renderStage->presenter->context->memoryManager->allocator);
            vkuDestroyImageView(renderStage->pTargetColorImgViews[0], renderStage->presenter->context->device);
        }

        if (renderStage->pTargetDepthImgViews[0] != NULL)
        {
            vkuDestroyImage(renderStage->pTargetDepthImages[0], renderStage->pTargetDepthImgAllocs[0], renderStage->presenter->context->memoryManager->allocator);
            vkuDestroyImageView(renderStage->pTargetDepthImgViews[0], renderStage->presenter->context->device);
        }
    }

    if ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER)
    {
        renderStage->pTargetColorImgViews = renderStage->presenter->swapchainImageViews;
    }
    else
    {
        if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE)
        {
            VkuVkImageCreateInfo imageCreateInfo = {
                .allocator = renderStage->presenter->context->memoryManager->allocator,
                .width = renderStage->presenter->swapchainExtend.width,
                .height = renderStage->presenter->swapchainExtend.height,
                .mipLevels = 1,
                .format = VK_FORMAT_B8G8R8A8_SRGB,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .numSamples = VK_SAMPLE_COUNT_1_BIT,
                .pImage = &(renderStage->pTargetColorImages[0]),
                .pImageAlloc = &(renderStage->pTargetColorImgAllocs[0]),
                .pImageAllocInfo = NULL,
                .arrayLayers = 1
            };

            vkuCreateImage(&imageCreateInfo);

            renderStage->pTargetColorImgViews[0] = vkuCreateImageView(renderStage->pTargetColorImages[0], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, renderStage->presenter->context->device);
        }

        if ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE && renderStage->enableDepthTesting)
        {
            VkuVkImageCreateInfo imageCreateInfo = {
                .allocator = renderStage->presenter->context->memoryManager->allocator,
                .width = renderStage->presenter->swapchainExtend.width,
                .height = renderStage->presenter->swapchainExtend.height,
                .mipLevels = 1,
                .format = VK_FORMAT_D32_SFLOAT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .numSamples = VK_SAMPLE_COUNT_1_BIT,
                .pImage = &(renderStage->pTargetDepthImages[0]),
                .pImageAlloc = &(renderStage->pTargetDepthImgAllocs[0]),
                .pImageAllocInfo = NULL,
                .arrayLayers = 1,
            };

            vkuCreateImage(&imageCreateInfo);

            renderStage->pTargetDepthImgViews[0] = vkuCreateImageView(renderStage->pTargetDepthImages[0], VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1, renderStage->presenter->context->device);
        }
    }

    if (renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT && ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE || (renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER))
    {
        renderStage->colorResource = vkuRenderResourceManagerGetColorResource(renderStage->presenter->resourceManager, renderStage->sampleCount);
    }

    if ((renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT || (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) != VKU_RENDER_OPTION_DEPTH_IMAGE) && renderStage->enableDepthTesting)
    {
        renderStage->depthResource = vkuRenderResourceManagerGetDepthResource(renderStage->presenter->resourceManager, renderStage->sampleCount);
    }

    VkuVkRenderPassCreateInfo renderPassCreateInfo = {
        .format = renderStage->presenter->swapchainFormat,
        .msaaSamples = renderStage->sampleCount,
        .physicalDevice = renderStage->presenter->context->physicalDevice,
        .device = renderStage->presenter->context->device,
        .enableDepthTest = renderStage->enableDepthTesting,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? true : ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? false : ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .presentationLayout = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER),
    };

    renderStage->renderPass = vkuCreateVkRenderPass(&renderPassCreateInfo);

    VkuVkFramebufferCreateInfo frameBufferCreateInfo = {
        .imageCount = renderStage->outputCount,
        .msaaSamples = renderStage->sampleCount,
        .renderStageColorImageView = (renderStage->colorResource) ? renderStage->colorResource->imageView : VK_NULL_HANDLE,
        .renderStageDepthImageView = (renderStage->depthResource) ? renderStage->depthResource->imageView : VK_NULL_HANDLE,
        .renderTargetColorImageViews = renderStage->pTargetColorImgViews,
        .renderTargetDepthImageViews = renderStage->pTargetDepthImgViews,
        .renderPass = renderStage->renderPass,
        .extend = renderStage->presenter->swapchainExtend,
        .device = renderStage->presenter->context->device,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? true : ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER) ? false : ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .enableDepthTest = renderStage->enableDepthTesting,
        .layerCount = 1};

    renderStage->framebuffers = vkuCreateVkFramebuffer(&frameBufferCreateInfo);

    for (uint32_t i = 0; i < renderStage->outputTextureManager->elemCnt; i++)
    {
        vkuRenderStageOutputTextureUpdate((VkuTexture2D)renderStage->outputTextureManager->elements[i]);
    }

    for (uint32_t i = 0; i < renderStage->pipelineManager->elemCnt; i++)
    {
        vkuPipelineUpdate((VkuPipeline)renderStage->pipelineManager->elements[i]);
    }

    for (uint32_t i = 0; i < renderStage->descriptorSetManager->elemCnt; i++)
    {
        vkuDescriptorSetUpdate((VkuDescriptorSet)renderStage->descriptorSetManager->elements[i]);
    }
}

void vkuDestroyRenderStage(VkuRenderStage renderStage)
{
    if (renderStage->staticRenderStage == VK_TRUE)
        EXIT("VkuError: Trying to destroy a VkuStaticRenderStage with vkuDestroyRenderStage()!\n"); 

    vkDeviceWaitIdle(renderStage->presenter->context->device);

    for (uint32_t i = 0; i < renderStage->outputTextureManager->elemCnt; i++)
    {
        VkuTexture2D outTex = (VkuTexture2D)renderStage->outputTextureManager->elements[i];
        free(outTex);
    }

    vkuDestroyObjectManager(renderStage->descriptorSetManager);
    vkuDestroyObjectManager(renderStage->outputTextureManager);
    vkuDestroyObjectManager(renderStage->pipelineManager);
    vkuDestroyFramebuffer(renderStage->presenter->context->device, renderStage->framebuffers, renderStage->outputCount);
    vkuDestroyVkRenderPass(renderStage->presenter->context->device, renderStage->renderPass);

    if ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) != VKU_RENDER_OPTION_PRESENTER)
    {
        if (renderStage->pTargetColorImgViews[0] != NULL)
        {
            vkuDestroyImage(renderStage->pTargetColorImages[0], renderStage->pTargetColorImgAllocs[0], renderStage->presenter->context->memoryManager->allocator);
            vkuDestroyImageView(renderStage->pTargetColorImgViews[0], renderStage->presenter->context->device);
        }

        if (renderStage->pTargetDepthImgViews[0] != NULL)
        {
            vkuDestroyImage(renderStage->pTargetDepthImages[0], renderStage->pTargetDepthImgAllocs[0], renderStage->presenter->context->memoryManager->allocator);
            vkuDestroyImageView(renderStage->pTargetDepthImgViews[0], renderStage->presenter->context->device);
        }

        free(renderStage->pTargetColorImages);
        free(renderStage->pTargetColorImgAllocs);
        free(renderStage->pTargetColorImgViews);
        free(renderStage->pTargetDepthImages);
        free(renderStage->pTargetDepthImgAllocs);
        free(renderStage->pTargetDepthImgViews);
    }

    vkuObjectManagerRemove(renderStage->presenter->renderStageManager, (void *)renderStage);
    free(renderStage);
}

// VkuStaticRenderStage

VkuRenderStage vkuCreateStaticRenderStage(VkuStaticRenderStageCreateInfo * createInfo)
{
    VkuRenderStage renderStage = (VkuRenderStage) calloc(1, sizeof(VkuRenderStage_T));
    renderStage->context = createInfo->context;
    renderStage->enableDepthTesting = createInfo->enableDepthTesting;
    renderStage->extend.width = createInfo->width;
    renderStage->extend.height = createInfo->height;
    renderStage->sampleCount = createInfo->msaaSamples;
    renderStage->options = createInfo->options;
    renderStage->staticRenderStage = VK_TRUE;
    renderStage->staticDepthArrayCount = createInfo->depthLayers;
    
    if ((renderStage->options & VKU_RENDER_OPTION_PRESENTER) == VKU_RENDER_OPTION_PRESENTER)
        EXIT("VkuError: A VkuStaticRenderStage cant be created with VKU_RENDER_OPTION_PRESENTER option enabled!");

    if (!renderStage->enableDepthTesting && (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE)
        EXIT("VkuError: RenderStage is created with VKU_RENDER_OPTION_DEPTH_IMAGE option enabled, but enableDepthTesting is disabled!\n");

    renderStage->pTargetColorImages = (VkImage *)calloc(1, sizeof(VkImage));
    renderStage->pTargetColorImgViews = (VkImageView *)calloc(1, sizeof(VkImageView));
    renderStage->pTargetColorImgAllocs = (VmaAllocation *)calloc(1, sizeof(VmaAllocation));
    renderStage->pTargetDepthImages = (VkImage *)calloc(1, sizeof(VkImage));
    renderStage->pTargetDepthImgViews = (VkImageView *)calloc(1, sizeof(VkImageView));
    renderStage->pTargetDepthImgAllocs = (VmaAllocation *)calloc(1, sizeof(VmaAllocation));

    if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE)
    {
        VkuVkImageCreateInfo imageCreateInfo = {
            .allocator = renderStage->context->memoryManager->allocator,
            .width = renderStage->extend.width,
            .height = renderStage->extend.height,
            .mipLevels = 1,
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .pImage = &(renderStage->pTargetColorImages[0]),
            .pImageAlloc = &(renderStage->pTargetColorImgAllocs[0]),
            .pImageAllocInfo = NULL,
            .arrayLayers = 1
        };

        vkuCreateImage(&imageCreateInfo);
        renderStage->pTargetColorImgViews[0] = vkuCreateImageView(renderStage->pTargetColorImages[0], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, renderStage->context->device);
    }

    if ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE && renderStage->enableDepthTesting)
    {
        VkuVkImageCreateInfo imageCreateInfo = {
            .allocator = renderStage->context->memoryManager->allocator,
            .width = renderStage->extend.width,
            .height = renderStage->extend.height,
            .mipLevels = 1,
            .format = VK_FORMAT_D32_SFLOAT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .numSamples = VK_SAMPLE_COUNT_1_BIT,
            .pImage = &(renderStage->pTargetDepthImages[0]),
            .pImageAlloc = &(renderStage->pTargetDepthImgAllocs[0]),
            .pImageAllocInfo = NULL,
            .arrayLayers = createInfo->depthLayers
        };

        vkuCreateImage(&imageCreateInfo);
        renderStage->pTargetDepthImgViews[0] = vkuCreateImageView(renderStage->pTargetDepthImages[0], VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, createInfo->depthLayers, renderStage->context->device);
    }

    if (renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT && ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE))
    {
        VkuColorResourcesCreateInfo colorInfo = {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .allocator = renderStage->context->memoryManager->allocator,
            .extend = renderStage->extend,
            .msaa_samples = renderStage->sampleCount,
            .device = renderStage->context->device,
        };

        renderStage->colorResource = vkuCreateColorResources(&colorInfo);
    }

    if ((renderStage->sampleCount != VK_SAMPLE_COUNT_1_BIT || (renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) != VKU_RENDER_OPTION_DEPTH_IMAGE) && renderStage->enableDepthTesting)
    {
        VkuDepthResourcesCreateInfo depthInfo = {
            .physical_device = renderStage->context->physicalDevice,
            .device = renderStage->context->device,
            .allocator = renderStage->context->memoryManager->allocator,
            .extend = renderStage->extend,
            .msaa_samples = renderStage->sampleCount,
            .layerCount = 1
        };

        renderStage->depthResource = vkuCreateDepthResources(&depthInfo);
    }

    VkuVkRenderPassCreateInfo renderPassCreateInfo = {
        .format = VK_FORMAT_B8G8R8A8_SRGB,
        .msaaSamples = renderStage->sampleCount,
        .physicalDevice = renderStage->context->physicalDevice,
        .device = renderStage->context->device,
        .enableDepthTest = renderStage->enableDepthTesting,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .presentationLayout = VK_FALSE,
    };

    renderStage->renderPass = vkuCreateVkRenderPass(&renderPassCreateInfo);

    VkuVkFramebufferCreateInfo frameBufferCreateInfo = {
        .imageCount = 1,
        .msaaSamples = renderStage->sampleCount,
        .renderStageColorImageView = (renderStage->colorResource) ? renderStage->colorResource->imageView : VK_NULL_HANDLE,
        .renderStageDepthImageView = (renderStage->depthResource) ? renderStage->depthResource->imageView : VK_NULL_HANDLE,
        .renderTargetColorImageViews = renderStage->pTargetColorImgViews,
        .renderTargetDepthImageViews = renderStage->pTargetDepthImgViews,
        .renderPass = renderStage->renderPass,
        .extend = renderStage->extend,
        .device = renderStage->context->device,
        .enableTargetColorImage = ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE) == VKU_RENDER_OPTION_COLOR_IMAGE),
        .enableTargetDepthImage = ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE),
        .enableDepthTest = renderStage->enableDepthTesting,
        .layerCount = createInfo->depthLayers};

    renderStage->framebuffers = vkuCreateVkFramebuffer(&frameBufferCreateInfo);

    return renderStage;
}

void vkuDestroyStaticRenderStage(VkuRenderStage renderStage)
{
    if (renderStage->staticRenderStage == VK_FALSE)
        EXIT("VkuError: Trying to destroy a VkuRenderStage with vkuDestroyStaticRenderStage()!\n"); 

    vkDeviceWaitIdle(renderStage->context->device);

    vkuDestroyFramebuffer(renderStage->context->device, renderStage->framebuffers, 1);
    vkuDestroyVkRenderPass(renderStage->context->device, renderStage->renderPass);

    if (renderStage->colorResource != NULL)
    {
        vkuDestroyColorResources(renderStage->context->memoryManager->allocator, renderStage->context->device, renderStage->colorResource);
    }

    if (renderStage->depthResource != NULL)
    {
        vkuDestroyDepthResources(renderStage->context->memoryManager->allocator, renderStage->context->device, renderStage->depthResource);
    }

    if (renderStage->pTargetColorImgViews[0] != NULL)
    {
        vkuDestroyImage(renderStage->pTargetColorImages[0], renderStage->pTargetColorImgAllocs[0], renderStage->context->memoryManager->allocator);
        vkuDestroyImageView(renderStage->pTargetColorImgViews[0], renderStage->context->device);
    }

    if (renderStage->pTargetDepthImgViews[0] != NULL)
    {
        vkuDestroyImage(renderStage->pTargetDepthImages[0], renderStage->pTargetDepthImgAllocs[0], renderStage->context->memoryManager->allocator);
        vkuDestroyImageView(renderStage->pTargetDepthImgViews[0], renderStage->context->device);
    }

    free(renderStage->pTargetColorImages);
    free(renderStage->pTargetColorImgAllocs);
    free(renderStage->pTargetColorImgViews);
    free(renderStage->pTargetDepthImages);
    free(renderStage->pTargetDepthImgAllocs);
    free(renderStage->pTargetDepthImgViews);

    free(renderStage);
}

void vkuRenderStageSetMSAA(VkuRenderStage renderStage, VkSampleCountFlagBits msaaFlags)
{
    if (renderStage->staticRenderStage == VK_FALSE)
    {
        renderStage->sampleCount = msaaFlags;
        vkuRenderStageUpdate(renderStage);

        for (uint32_t i = 0; i < renderStage->presenter->renderStageManager->elemCnt; i++)
        {
            vkuRenderStageUpdate((VkuRenderStage) renderStage->presenter->renderStageManager->elements[i]);
        }
    } else {
        EXIT("VkuError: vkuRenderStageSetMSAA() cant be used with a static VkuRenderStage!\n");
    }
}

// VkuFrame

VkuFrame vkuPresenterBeginFrame(VkuPresenter presenter)
{
    if (presenter->activeFrame)
        EXIT("VkuError: Only One Frame can be active at a time!\n");
    else
        presenter->activeFrame = VK_TRUE;

    VkuFrame_T *frame = (VkuFrame_T *)calloc(1, sizeof(VkuFrame_T));

    frame->presenter = presenter;

    int currentFrame = frame->presenter->currentFrame;
    VkuContext context = presenter->context;

    vkWaitForFences(context->device, 1, &frame->presenter->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    uint32_t imageIndex = 0;

    VkResult result = vkAcquireNextImageKHR(context->device, frame->presenter->swapchain, UINT64_MAX, frame->presenter->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vkuPresenterRecreate(presenter);
        presenter->window->window_resized = false;
        return vkuPresenterBeginFrame(presenter);
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        EXIT("failed to acquire swap chain image!");
    }

    vkResetFences(context->device, 1, &frame->presenter->inFlightFences[currentFrame]);

    vkResetCommandBuffer(frame->presenter->cmdBuffer[currentFrame], 0);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(frame->presenter->cmdBuffer[currentFrame], &beginInfo));

    frame->imageIndex = imageIndex;
    frame->cmdBuffer = frame->presenter->cmdBuffer[currentFrame];
    frame->activeRenderStage = false;

    return frame;
}

void vkuPresenterSubmitFrame(VkuFrame frame, VkuComputeRun syncComputeRun)
{
    int currentFrame = frame->presenter->currentFrame;
    int imageIndex = frame->imageIndex;

    VK_CHECK(vkEndCommandBuffer(frame->presenter->cmdBuffer[currentFrame]));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[2] = {frame->presenter->imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[2] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;

    if (syncComputeRun != NULL) {
        waitSemaphores[1] = syncComputeRun->executor->computeFinishedSemaphores[syncComputeRun->lastFrame];
        waitStages[1] = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        submitInfo.waitSemaphoreCount = 2;
    }

    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(frame->presenter->cmdBuffer)[currentFrame];

    VkSemaphore signalSemaphores[] = {frame->presenter->renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(frame->presenter->context->graphicsQueue, 1, &submitInfo, frame->presenter->inFlightFences[currentFrame]));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {frame->presenter->swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = (const uint32_t *)&imageIndex;
    presentInfo.pResults = NULL;

    VkResult result = vkQueuePresentKHR(frame->presenter->context->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame->presenter->window->window_resized)
    {
        vkuPresenterRecreate(frame->presenter);
        frame->presenter->window->window_resized = false;
    }
    else if (result != VK_SUCCESS)
        EXIT("Failed to present swap chain image!\n");

    (frame->presenter->currentFrame) = (currentFrame + 1) % frame->presenter->framesInFlight;
    frame->presenter->activeFrame = VK_FALSE;

    free(frame);
}

void vkuFrameBeginRenderStage(VkuFrame frame, VkuRenderStage renderStage)
{
    if (frame->activeRenderStage)
        EXIT("VkuError: Only one VkuRenderStage can be active at a time!\n");
    else
        frame->activeRenderStage = true;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (renderStage->staticRenderStage == VK_FALSE) ? (float) frame->presenter->swapchainExtend.width : (float) renderStage->extend.width;
    viewport.height = (renderStage->staticRenderStage == VK_FALSE) ? (float) frame->presenter->swapchainExtend.height : (float) renderStage->extend.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(frame->presenter->cmdBuffer[frame->presenter->currentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = (renderStage->staticRenderStage == VK_FALSE) ? frame->presenter->swapchainExtend : renderStage->extend;
    vkCmdSetScissor(frame->presenter->cmdBuffer[frame->presenter->currentFrame], 0, 1, &scissor); 

    VkClearValue clearValues[2] = {{}, {}};
    int clearValueCnt = 0;
    if ((renderStage->options & VKU_RENDER_OPTION_COLOR_IMAGE))
        clearValues[clearValueCnt++].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 0.0f}};
    if ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE))
        clearValues[clearValueCnt++].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderStage->renderPass;
    renderPassInfo.framebuffer = renderStage->framebuffers[((renderStage->options & VKU_RENDER_OPTION_PRESENTER) != VKU_RENDER_OPTION_PRESENTER) ? 0 : frame->imageIndex];
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = (renderStage->staticRenderStage == VK_FALSE) ? frame->presenter->swapchainExtend : renderStage->extend;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(frame->presenter->cmdBuffer[frame->presenter->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void vkuFrameFinishRenderStage(VkuFrame frame, VkuRenderStage renderStage)
{
    if (!frame->activeRenderStage)
        EXIT("VkuError: No active RenderStage!\n");
    else
        frame->activeRenderStage = false;

    vkCmdEndRenderPass(frame->presenter->cmdBuffer[frame->presenter->currentFrame]);

    if (renderStage->sampleCount == VK_SAMPLE_COUNT_1_BIT && ((renderStage->options & VKU_RENDER_OPTION_DEPTH_IMAGE) == VKU_RENDER_OPTION_DEPTH_IMAGE))
    {
        vkuTransitionDepthImageLayout(frame->presenter->context->device, frame->presenter->cmdBuffer[frame->presenter->currentFrame], renderStage->pTargetDepthImages[0], renderStage->staticDepthArrayCount);
    }
}

void vkuFrameBindPipeline(VkuFrame frame, VkuPipeline pipeline)
{
    vkCmdBindPipeline(frame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

    if (pipeline->descriptorSet != NULL)
        vkCmdBindDescriptorSets(frame->cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &(pipeline->descriptorSet->sets)[frame->presenter->currentFrame], 0, NULL);
}

void vkuFramePipelinePushConstant(VkuFrame frame, VkuPipeline pipeline, void *data, size_t size)
{
    if (size > 128)
        EXIT("VkuError: PushConstant size is bigger than the max allowed Value of 128 bytes!\n");

    vkCmdPushConstants(frame->cmdBuffer, pipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, size, data);
}

void vkuFrameDrawVertexBuffer(VkuFrame frame, VkuBuffer buffer, uint64_t vertexCount, uint32_t instanceCount, uint32_t firstVertex)
{
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(frame->cmdBuffer, 0, 1, &buffer->buffer, offsets);
    vkCmdDraw(frame->cmdBuffer, vertexCount, instanceCount, firstVertex, 0);
}

void vkuFrameUpdateUniformBuffer(VkuFrame frame, VkuUniformBuffer uniBuffer, void *data)
{
    memcpy(uniBuffer->mappedMemory[frame->presenter->currentFrame], data, uniBuffer->bufferSize);
}

void vkuFrameDrawVoid(VkuFrame frame, uint64_t vertexCount)
{
    vkCmdDraw(frame->cmdBuffer, vertexCount, 1, 0, 0);
}

// VkuTexture2D

VkuTexture2D vkuCreateTexture2D(VkuContext context, VkuTexture2DCreateInfo *createInfo)
{
    VkuTexture2D_T *texture = (VkuTexture2D_T *)calloc(1, sizeof(VkuTexture2D_T));
    texture->renderStage = NULL;
    texture->renderStageColorImage = VK_FALSE;
    texture->renderStageDepthImage = VK_FALSE;
    texture->imageExtend.height = createInfo->height;
    texture->imageExtend.width = createInfo->width;

    VkuTextureImageCreateInfo texInfo = {
        .textureData = createInfo->pixelData,
        .texWidth = createInfo->width,
        .texHeight = createInfo->height,
        .mipLevels = (uint32_t)createInfo->mipLevels,
        .allocator = context->memoryManager->allocator,
        .pTexImage = &texture->textureImage,
        .pTexImageAlloc = &texture->textureImageAllocation,
        .device = context->device,
        .physicalDevice = context->physicalDevice,
        .cmdPool = context->graphicsCmdPool,
        .graphicsQueue = context->graphicsQueue,
    };

    vkuCreateTextureImage(&texInfo);
    texture->textureImageView = vkuCreateTextureImageView(context->device, texture->textureImage, createInfo->mipLevels);

    return texture;
}

void vkuDestroyTexture2D(VkuContext context, VkuTexture2D texture)
{
    if (!texture->renderStage)
    {
        vkuDestroyTextureImageView(context->device, texture->textureImageView);
        vkuDestroyTextureImage(context->memoryManager->allocator, texture->textureImage, texture->textureImageAllocation);
    }

    free(texture);
}

// VkuTexture2DArray

VkuTexture2DArray vkuCreateTexture2DArray(VkuContext context, VkuTexture2DArrayCreateInfo *createInfo)
{
    VkuTexture2DArray_T *texArray = (VkuTexture2DArray_T *)calloc(1, sizeof(VkuTexture2DArray_T));

    VkuTextureImageArrayCreateInfo texInfo = {
        .width = (uint32_t)createInfo->width,
        .height = (uint32_t)createInfo->height,
        .layers = (uint32_t)createInfo->layerCount,
        .tex_data = createInfo->pixelDataArray,
        .mip_levels = (uint32_t)createInfo->mipLevels,
        .allocator = context->memoryManager->allocator,
        .pTexArrayImg = &texArray->textureImage,
        .pTexArrayAlloc = &texArray->textureImageAllocation,
        .device = context->device,
        .cmd_pool = context->graphicsCmdPool,
        .graphics_queue = context->graphicsQueue,
        .physical_device = context->physicalDevice,
    };

    vkuCreateTextureImageArray(&texInfo);
    texArray->textureImageView = vkuCreateTextureImageArrayView(context->device, texArray->textureImage, createInfo->mipLevels, createInfo->layerCount);

    return texArray;
}

void vkuDestroyTexture2DArray(VkuContext context, VkuTexture2DArray texArray)
{
    vkuDestroyTextureImageView(context->device, texArray->textureImageView);
    vkuDestroyTextureImage(context->memoryManager->allocator, texArray->textureImage, texArray->textureImageAllocation);

    free(texArray);
}

// VkuTextureSampler

VkuTextureSampler vkuCreateTextureSampler(VkuContext context, VkuTextureSamplerCreateInfo *createInfo)
{
    VkuTextureSampler_T *sampler = (VkuTextureSampler_T *)calloc(1, sizeof(VkuTextureSampler_T));

    VkuSamplerCreateInfo samplerInfo = {
        .minFilter = createInfo->minFilter,
        .magFilter = createInfo->magFilter,
        .repeatMode = createInfo->repeatMode,
        .mipmapLevels = createInfo->mipmapLevels,
        .physicalDevice = context->physicalDevice,
        .device = context->device,
        .maxAnisotropy = 16.0f,
        .enableCompare = createInfo->enableCompare,
        .compareOp = createInfo->compareOp,
        .borderColor = createInfo->borderColor};

    sampler->sampler = vkuCreateSampler(&samplerInfo);

    return sampler;
}

void vkuDestroyTextureSampler(VkuContext context, VkuTextureSampler sampler)
{
    vkuDestroySampler(sampler->sampler, context->device);
    free(sampler);
}

// VkuUniformBuffer

VkuUniformBuffer vkuCreateUniformBuffer(VkuContext context, VkDeviceSize bufferSize, uint32_t count)
{
    if (bufferSize <= 0)
        EXIT("VkuError: Invalid UniformBuffer size. must not be 0");

    VkuUniformBuffer_T *uniBuffer = (VkuUniformBuffer_T *)calloc(1, sizeof(VkuUniformBuffer_T));
    uniBuffer->bufferCount = count;
    uniBuffer->bufferSize = bufferSize;

    VkuUniformBuffersCreateInfo uniInfo = {
        .ppUniformBuffer = &uniBuffer->uniformBuffer,
        .ppUniformAllocations = &uniBuffer->uniformAllocs,
        .pppMappedUniformMemory = &uniBuffer->mappedMemory,
        .allocator = context->memoryManager->allocator,
        .bufferSize = bufferSize,
        .bufferCount = count,
    };

    vkuCreateUniformBuffers(&uniInfo);

    return uniBuffer;
}

void vkuDestroyUniformBuffer(VkuContext context, VkuUniformBuffer uniformBuffer)
{
    vkuDestroyUniformBuffers(context->memoryManager->allocator, uniformBuffer->uniformBuffer, uniformBuffer->uniformAllocs, uniformBuffer->mappedMemory, uniformBuffer->bufferCount);
    free(uniformBuffer);
}

void vkuUpdateUniformBuffer(VkuFrame frame, VkuUniformBuffer uniBuffer, void *data, uint32_t bufferIndex)
{
    memcpy(uniBuffer->mappedMemory[bufferIndex], data, uniBuffer->bufferSize);
}


// VkuDescriptorSet

VkuDescriptorSet vkuCreateDescriptorSet(VkuDescriptorSetCreateInfo *createInfo)
{
    VkuDescriptorSet_T *set = (VkuDescriptorSet_T *)calloc(1, sizeof(VkuDescriptorSet_T));
    set->renderStage = createInfo->renderStage;
    set->context = createInfo->context;
    set->setCount = createInfo->descriptorCount;

    set->setLayout = vkuCreateDescriptorSetLayout(set->context->device, createInfo->attributes, createInfo->attributeCount);
    set->pool = vkuCreateDescriptorPool(set->context->device, createInfo->attributes, createInfo->attributeCount, set->setCount);

    VkuDescriptorSetsCreateInfo setsInfo = {
        .setCount = set->setCount,
        .setLayout = set->setLayout,
        .pool = set->pool,
        .device = set->context->device,
        .attributes = createInfo->attributes,
        .attribCount = createInfo->attributeCount,
    };

    set->sets = vkuCreateDescriptorSets(&setsInfo);
    set->attributeCount = createInfo->attributeCount;
    set->attributes = (VkuDescriptorSetAttribute *)malloc(sizeof(VkuDescriptorSetAttribute) * createInfo->attributeCount);

    for (uint32_t i = 0; i < set->attributeCount; i++)
    {
        set->attributes[i].type = createInfo->attributes[i].type;
        set->attributes[i].sampler = createInfo->attributes[i].sampler;
        set->attributes[i].tex2D = createInfo->attributes[i].tex2D;
        set->attributes[i].tex2DArray = createInfo->attributes[i].tex2DArray;
        set->attributes[i].uniformBuffer = createInfo->attributes[i].uniformBuffer;
        set->attributes[i].shaderStage = createInfo->attributes[i].shaderStage;
    }

    if (set->renderStage != NULL && set->renderStage->staticRenderStage == VK_FALSE)
        vkuObjectManagerAdd(set->renderStage->descriptorSetManager, (void *)set);

    return set;
}

void vkuDescriptorSetUpdate(VkuDescriptorSet descriptorSet)
{
    vkuDestroyDescriptorSets(descriptorSet->sets);
    vkuDestroyDescriptorPool(descriptorSet->renderStage->context->device, descriptorSet->pool);
    vkuDestroyDescriptorSetLayout(descriptorSet->renderStage->context->device, descriptorSet->setLayout);

    descriptorSet->setLayout = vkuCreateDescriptorSetLayout(descriptorSet->renderStage->context->device, descriptorSet->attributes, descriptorSet->attributeCount);
    descriptorSet->pool = vkuCreateDescriptorPool(descriptorSet->renderStage->context->device, descriptorSet->attributes, descriptorSet->attributeCount, descriptorSet->setCount);

    VkuDescriptorSetsCreateInfo setsInfo = {
        .setCount = descriptorSet->setCount,
        .setLayout = descriptorSet->setLayout,
        .pool = descriptorSet->pool,
        .device = descriptorSet->renderStage->context->device,
        .attributes = descriptorSet->attributes,
        .attribCount = descriptorSet->attributeCount,
    };

    descriptorSet->sets = vkuCreateDescriptorSets(&setsInfo);
}

void vkuDestroyDescriptorSet(VkuDescriptorSet set)
{
    if (set->renderStage != NULL)
        vkuObjectManagerRemove(set->renderStage->descriptorSetManager, (void *)set);
    vkuDestroyDescriptorSets(set->sets);
    vkuDestroyDescriptorPool(set->context->device, set->pool);
    vkuDestroyDescriptorSetLayout(set->context->device, set->setLayout);
    free(set->attributes);
    free(set);
}

// VkuPipeline

VkuPipeline vkuCreatePipeline(VkuContext context, VkuPipelineCreateInfo *createInfo)
{
    if (!createInfo->renderStage->enableDepthTesting && createInfo->depthTestEnable)
        EXIT("VkuError: Pipeline Creation: RenderStage does not support DepthTesting!");

    VkuPipeline_T *pipeline = (VkuPipeline_T *)calloc(1, sizeof(VkuPipeline_T));
    if (!pipeline)
    {
        return NULL;
    }

    memcpy(&pipeline->recreateInfo, createInfo, sizeof(VkuPipelineCreateInfo));

    pipeline->descriptorSet = createInfo->descriptorSet;
    pipeline->renderStage = createInfo->renderStage;

    size_t vertexShaderLength = createInfo->vertexShaderLength;
    size_t fragmentShaderLength = createInfo->fragmentShaderLength;

    pipeline->internalVertexSpirv = (char *)malloc((vertexShaderLength) * sizeof(char));
    memcpy(pipeline->internalVertexSpirv, createInfo->vertexShaderSpirV, vertexShaderLength);

    if (createInfo->fragmentShaderSpirV != NULL && createInfo->fragmentShaderLength > 0) {
        pipeline->internalFragmentSpirv = (char *)malloc((fragmentShaderLength) * sizeof(char));
        memcpy(pipeline->internalFragmentSpirv, createInfo->fragmentShaderSpirV, fragmentShaderLength);
    } else {
        pipeline->internalFragmentSpirv = NULL;
    }

    pipeline->vertexAttributes = (VkuVertexAttribute *)malloc(sizeof(VkuVertexAttribute) * createInfo->vertexLayout.attributeCount);
    for (uint32_t i = 0; i < createInfo->vertexLayout.attributeCount; i++)
        memcpy(&pipeline->vertexAttributes[i], &createInfo->vertexLayout.attributes[i], sizeof(VkuVertexAttribute));
    pipeline->vertexLayout.attributeCount = createInfo->vertexLayout.attributeCount;
    pipeline->vertexLayout.attributes = pipeline->vertexAttributes;
    pipeline->vertexLayout.vertexSize = createInfo->vertexLayout.vertexSize;

    pipeline->pipelineLayout = vkuCreatePipelineLayout(context->device, &createInfo->descriptorSet->setLayout, (createInfo->descriptorSet != NULL) ? 1 : 0);

    VkuGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .device = context->device,
        .vertexShaderSpirv = pipeline->internalVertexSpirv,
        .vertexShaderLength = (uint32_t)vertexShaderLength,
        .fragmentShaderSpirv = pipeline->internalFragmentSpirv,
        .fragmentShaderLength = (uint32_t)fragmentShaderLength,
        .vertexInputLayout = createInfo->vertexLayout,
        .swapchainExtend = (pipeline->renderStage->staticRenderStage == VK_FALSE) ? pipeline->renderStage->presenter->swapchainExtend : pipeline->renderStage->extend,
        .polygonMode = createInfo->polygonMode,
        .pipelineLayout = pipeline->pipelineLayout,
        .renderPass = pipeline->renderStage->renderPass,
        .msaaSamples = pipeline->renderStage->sampleCount,
        .depth_test_write = createInfo->depthTestWrite,
        .depth_test_enable = createInfo->depthTestEnable,
        .depth_compare_mode = createInfo->depthCompareMode,
        .cullMode = createInfo->cullMode,
        .enableDepthBias = createInfo->enableDepthBias,
        .depthBiasConstantFactor = createInfo->depthBiasConstantFactor,
        .depthBiasSlopeFactor = createInfo->depthBiasSlopeFactor,
        .topology = createInfo->topology
    };

    pipeline->graphicsPipeline = vkuCreateGraphicsPipeline(&pipelineCreateInfo);

    if (pipeline->renderStage->staticRenderStage == VK_FALSE)
        vkuObjectManagerAdd(pipeline->renderStage->pipelineManager, (void *)pipeline);

    return pipeline;
}

void vkuPipelineUpdate(VkuPipeline pipeline)
{
    vkuDestroyVkPipeline(pipeline->renderStage->context->device, pipeline->graphicsPipeline);

    VkuGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .device = pipeline->renderStage->context->device,
        .vertexShaderSpirv = pipeline->internalVertexSpirv,
        .vertexShaderLength = pipeline->recreateInfo.vertexShaderLength,
        .fragmentShaderSpirv = pipeline->internalFragmentSpirv,
        .fragmentShaderLength = pipeline->recreateInfo.fragmentShaderLength,
        .vertexInputLayout = pipeline->vertexLayout,
 
        .swapchainExtend = (pipeline->renderStage->staticRenderStage == VK_FALSE) ? pipeline->renderStage->presenter->swapchainExtend : pipeline->renderStage->extend,
        .polygonMode = pipeline->recreateInfo.polygonMode,
        .pipelineLayout = pipeline->pipelineLayout,
        .renderPass = pipeline->renderStage->renderPass,
        .msaaSamples = pipeline->renderStage->sampleCount,
        .depth_test_write = pipeline->recreateInfo.depthTestWrite,
        .depth_test_enable = pipeline->recreateInfo.depthTestEnable,
        .depth_compare_mode = pipeline->recreateInfo.depthCompareMode,
        .cullMode = pipeline->recreateInfo.cullMode,
        .enableDepthBias = pipeline->recreateInfo.enableDepthBias,
        .depthBiasConstantFactor = pipeline->recreateInfo.depthBiasConstantFactor,
        .depthBiasSlopeFactor = pipeline->recreateInfo.depthBiasSlopeFactor,
        .topology = pipeline->recreateInfo.topology};

    pipeline->graphicsPipeline = vkuCreateGraphicsPipeline(&pipelineCreateInfo);
}

void vkuDestroyPipeline(VkuContext context, VkuPipeline pipeline)
{
    vkDeviceWaitIdle(context->device);

    vkuObjectManagerRemove(pipeline->renderStage->pipelineManager, (void *)pipeline);

    free(pipeline->internalVertexSpirv);
    if (pipeline->internalFragmentSpirv != NULL)
        free(pipeline->internalFragmentSpirv);

    free(pipeline->vertexAttributes);
    vkuDestroyVkPipeline(context->device, pipeline->graphicsPipeline);
    vkuDestroyPipelineLayout(context->device, pipeline->pipelineLayout);
    free(pipeline);
}

VkuComputeExecutor vkuCreateComputeExecutor(VkuContext context, uint32_t framesInFlight) {
    VkuComputeExecutor executor = calloc(1, sizeof(VkuComputeExecutor_T));
    executor->currentFrame = 0;
    executor->context = context;
    executor->framesInFlight = framesInFlight;
    executor->computeFinishedSemaphores = malloc(sizeof(VkSemaphore) * framesInFlight);
    executor->computeInFlightFences = malloc(sizeof(VkFence) *framesInFlight);
    executor->computeCommandBuffers = malloc(sizeof(VkCommandBuffer) *framesInFlight);
    executor->activeComputeRun = malloc(sizeof(VkuComputeRun_T));

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < framesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(context->device, &semaphoreInfo, NULL, &(executor->computeFinishedSemaphores[i])));
        VK_CHECK(vkCreateFence(context->device, &fenceInfo, NULL, &(executor->computeInFlightFences[i])));
    }

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = context->computeCmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) framesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(context->device, &allocInfo, executor->computeCommandBuffers));

    return executor;
}

void vkuDestroyComputeExecutor(VkuComputeExecutor computeExecutor) {
    for (size_t i = 0; i < computeExecutor->framesInFlight; i++) {
        vkDestroySemaphore(computeExecutor->context->device, computeExecutor->computeFinishedSemaphores[i], NULL);
        vkDestroyFence(computeExecutor->context->device, computeExecutor->computeInFlightFences[i], NULL);
    }

    free(computeExecutor->activeComputeRun);
    free(computeExecutor->computeCommandBuffers);
    free(computeExecutor->computeFinishedSemaphores);
    free(computeExecutor->computeInFlightFences);
    free(computeExecutor);
}

VkuComputeRun vkuComputeExecutorStartRun(VkuComputeExecutor executor) {
    if (executor->activeRun)
        EXIT("VkuError: Only one VkuComputeRun can be active at the same time!");
    executor->activeRun = VK_TRUE;


    VkuComputeRun run = executor->activeComputeRun;
    memset(run, 0, sizeof(VkuComputeRun_T));
    run->executor = executor;

    vkWaitForFences(executor->context->device, 1, &executor->computeInFlightFences[executor->currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(executor->context->device, 1, &executor->computeInFlightFences[executor->currentFrame]);
    vkResetCommandBuffer(executor->computeCommandBuffers[executor->currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo= {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(executor->computeCommandBuffers[executor->currentFrame], &beginInfo));

    return run;
}

void vkuComputeExecutorFinishRun(VkuComputeRun computeRun, VkBool32 enableFrameSyncronization) {
    VK_CHECK(vkEndCommandBuffer(computeRun->executor->computeCommandBuffers[computeRun->executor->currentFrame]));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &computeRun->executor->computeCommandBuffers[computeRun->executor->currentFrame];

    if (enableFrameSyncronization) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &computeRun->executor->computeFinishedSemaphores[computeRun->executor->currentFrame];
    }

    VK_CHECK(vkQueueSubmit(computeRun->executor->context->computeQueue, 1, &submitInfo, computeRun->executor->computeInFlightFences[computeRun->executor->currentFrame]));

    computeRun->lastFrame = computeRun->executor->currentFrame;
    computeRun->executor->currentFrame = (computeRun->executor->currentFrame + 1) % computeRun->executor->framesInFlight;

    computeRun->executor->activeRun = false;
}

void vkuComputeRunBindComputePipeline(VkuComputeRun computeRun, VkuComputePipeline pipeline, uint32_t dynamicOffsetCount, uint32_t * dynamicOffsets) {
    vkCmdBindPipeline(computeRun->executor->computeCommandBuffers[computeRun->executor->currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->computePipeline);
    vkCmdBindDescriptorSets(computeRun->executor->computeCommandBuffers[computeRun->executor->currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipelineLayout, 0, 1, &pipeline->descriptorSet->sets[computeRun->executor->currentFrame], dynamicOffsetCount, dynamicOffsets);
}

void vkuComputeRunDispatch(VkuComputeRun computeRun, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    vkCmdDispatch(computeRun->executor->computeCommandBuffers[computeRun->executor->currentFrame], groupCountX, groupCountY, groupCountZ);
}

void vkuComputeRunUpdateUniformBuffer(VkuComputeRun computeRun, VkuUniformBuffer uniBuffer, void *data)
{
    memcpy(uniBuffer->mappedMemory[computeRun->executor->currentFrame], data, uniBuffer->bufferSize);
}

VkuComputePipeline vkuCreateComputePipeline(VkuContext context, VkuComputePipelineCreateInfo *createInfo) {
    VkuComputePipeline_T *pipeline = (VkuComputePipeline_T *)calloc(1, sizeof(VkuComputePipeline_T));

    pipeline->descriptorSet = createInfo->descriptorSet;
    pipeline->internalComputeSpirv = (char *)malloc((createInfo->computeShaderLength) * sizeof(char));
    memcpy(pipeline->internalComputeSpirv, createInfo->computeShaderSpirV, createInfo->computeShaderLength * sizeof(char));

    pipeline->pipelineLayout = vkuCreatePipelineLayout(context->device, &createInfo->descriptorSet->setLayout, (createInfo->descriptorSet != NULL) ? 1 : 0);

    VkuComputeVkPipelineCreateInfo computePipelineCreateInfo = {
        .computeShaderLength = createInfo->computeShaderLength,
        .computeShaderSpirv = createInfo->computeShaderSpirV,
        .device = context->device,
        .pipelineLayout = pipeline->pipelineLayout
    };

    pipeline->computePipeline = vkuCreateComputeVkPipeline(&computePipelineCreateInfo);

    return pipeline;
}

void vkuDestroyComputePipeline(VkuContext context, VkuComputePipeline computePipeline) {
    vkuDestroyVkPipeline(context->device, computePipeline->computePipeline);
    vkuDestroyPipelineLayout(context->device, computePipeline->pipelineLayout);
    free(computePipeline->internalComputeSpirv);
    free(computePipeline);
}