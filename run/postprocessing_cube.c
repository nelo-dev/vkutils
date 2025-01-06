#include <stdio.h>
#include "../src/vkutils.h"

typedef struct ubo {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 2.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 2.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 2.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 2.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 2.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 2.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 3.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 3.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 3.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 3.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 3.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 3.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 4.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 4.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 4.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 4.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 4.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 4.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 5.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 5.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 5.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 5.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 5.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 5.0f,
};

int main()
{
    VkuContextCreateInfo contextCreateInfo = {
        .enableValidation = VK_TRUE,
        .applicationName = "VkuTest",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .usage = VKU_CONTEXT_USAGE_PRESENTATION};

    VkuContext context = vkuCreateContext(&contextCreateInfo);

    VkuPresenterCreateInfo presenterCreateInfo = {
        .context = context,
        .width = 1152,
        .height = 720,
        .windowTitle = "VkuTest",
        .windowIconPath = "./resources/icon.png",
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .framesInFlight = 2,
    };

    VkuPresenter presenter = vkuCreatePresenter(&presenterCreateInfo); //Must be created immediately after context creation!

    VkuWindow window = vkuPresenterGetWindow(presenter);
    VkuMemoryManager memoryManager = vkuContextGetMemoryManager(context); //Must be set AFTER vkuPresenter is created!

    VkuBuffer stagingBuffer = vkuCreateVertexBuffer(memoryManager, sizeof(cubeVertices), VKU_BUFFER_USAGE_CPU_TO_GPU);
    vkuSetVertexBufferData(memoryManager, stagingBuffer, cubeVertices, sizeof(cubeVertices));

    VkuBuffer vertexBuffer = vkuCreateVertexBuffer(memoryManager, sizeof(cubeVertices), VKU_BUFFER_USAGE_GPU_ONLY);
    VkDeviceSize sizes[] = {sizeof(cubeVertices)};
    vkuCopyBuffer(memoryManager, &stagingBuffer, &vertexBuffer, sizes, 1);

    vkuDestroyVertexBuffer(stagingBuffer, memoryManager, VK_TRUE);

    VkuStaticRenderStageCreateInfo staticRenderStageCreateInfo = {
        .context = context,
        .width = 288,
        .height = 180,
        .options = VKU_RENDER_OPTION_COLOR_IMAGE | VKU_RENDER_OPTION_DEPTH_IMAGE,
        .enableDepthTesting = VK_TRUE,
        .msaaSamples = vkuContextGetMaxSampleCount(context),
    };

    VkuRenderStage renderStage = vkuCreateStaticRenderStage(&staticRenderStageCreateInfo);

    int channels, height, width;
    uint8_t *images[] = {
        vkuLoadImage("./resources/textures/dirt.png", &width, &height, &channels),
        vkuLoadImage("./resources/textures/grass_top.png", &width, &height, &channels),
        vkuLoadImage("./resources/textures/grass_side.png", &width, &height, &channels),
        vkuLoadImage("./resources/textures/grass_side.png", &width, &height, &channels),
        vkuLoadImage("./resources/textures/grass_side.png", &width, &height, &channels),
        vkuLoadImage("./resources/textures/grass_side.png", &width, &height, &channels)};

    VkuTexture2DArrayCreateInfo texArrayCreateInfo = {
        .width = width,
        .height = height,
        .channels = channels,
        .layerCount = sizeof(images) / sizeof(uint8_t *),
        .mipLevels = 4,
        .pixelDataArray = images,
    };

    VkuTexture2DArray texArray = vkuCreateTexture2DArray(context, &texArrayCreateInfo);

    for (uint32_t i = 0; i < sizeof(images) / sizeof(uint8_t *); i++)
        free(images[i]);

    VkuTextureSamplerCreateInfo samplerCreateInfo = {
        .minFilter = VK_FILTER_NEAREST,
        .magFilter = VK_FILTER_NEAREST,
        .repeatMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipmapLevels = 4,
    };

    VkuTextureSampler sampler = vkuCreateTextureSampler(context, &samplerCreateInfo);

    VkuUniformBuffer uniformBuffer = vkuCreateUniformBuffer(context, sizeof(ubo), vkuPresenterGetFramesInFlight(presenter));

    VkuDescriptorSetAttribute setAttrib1 = {.type = VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER, .sampler = sampler, .tex2DArray = texArray};
    VkuDescriptorSetAttribute setAttrib2 = {.type = VKU_DESCRIPTOR_SET_ATTRIB_UNIFORM_BUFFER, .uniformBuffer = uniformBuffer};
    VkuDescriptorSetAttribute setAttribs[] = {setAttrib1, setAttrib2};
    VkuDescriptorSetCreateInfo descriptorSetCreateInfo = {
        .attributes = setAttribs,
        .attributeCount = 2,
        .renderStage = renderStage,
        .descriptorCount = vkuPresenterGetFramesInFlight(presenter)};

    VkuDescriptorSet descriptorSet = vkuCreateDescriptorSet(&descriptorSetCreateInfo);

    VkuVertexAttribute attrib1 = {.format = VK_FORMAT_R32G32B32_SFLOAT, .offset = 0};
    VkuVertexAttribute attrib2 = {.format = VK_FORMAT_R32G32_SFLOAT, .offset = sizeof(float) * 3};
    VkuVertexAttribute attrib3 = {.format = VK_FORMAT_R32_SFLOAT, .offset = 5 * sizeof(float)};
    VkuVertexAttribute vertexAttribs[] = {attrib1, attrib2, attrib3};
    VkuVertexLayout vertexLayout = {.attributeCount = 3, .attributes = vertexAttribs, .vertexSize = 6 * sizeof(float)};
    VkuPipelineCreateInfo pipelineCreateInfo = {
        .vertexShaderSpirV = vkuReadFile("./shader/simple_vert.spv", &pipelineCreateInfo.vertexShaderLength),
        .fragmentShaderSpirV = vkuReadFile("./shader/simple_frag.spv", &pipelineCreateInfo.fragmentShaderLength),
        .vertexLayout = vertexLayout,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .descriptorSet = descriptorSet,
        .depthTestWrite = VK_TRUE,
        .depthTestEnable = VK_TRUE,
        .depthCompareMode = VK_COMPARE_OP_LESS,
        .cullMode = VK_CULL_MODE_FRONT_BIT,
        .renderStage = renderStage};

    VkuPipeline pipeline = vkuCreatePipeline(context, &pipelineCreateInfo); 

    free(pipelineCreateInfo.fragmentShaderSpirV);
    free(pipelineCreateInfo.vertexShaderSpirV);

    VkuTexture2D colorOutput = vkuRenderStageGetColorOutput(renderStage);
    VkuTexture2D depthOutput = vkuRenderStageGetDepthOutput(renderStage);

    VkuRenderStageCreateInfo ppRenderStageCreateInfo = {
        .msaaSamples = vkuContextGetMaxSampleCount(context),
        .presenter = presenter,
        .options = VKU_RENDER_OPTION_PRESENTER,
        .enableDepthTesting = VK_FALSE};

    VkuRenderStage postRenderStage = vkuCreateRenderStage(&ppRenderStageCreateInfo);

    VkuDescriptorSetAttribute ppSetAttrib1 = {.type = VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER, .sampler = sampler, .tex2D = colorOutput};
    VkuDescriptorSetAttribute ppSetAttrib2 = {.type = VKU_DESCRIPTOR_SET_ATTRIB_SAMPLER, .sampler = sampler, .tex2D = depthOutput};
    VkuDescriptorSetAttribute ppSetAttribs[] = {ppSetAttrib1, ppSetAttrib2};
    VkuDescriptorSetCreateInfo ppDescriptorSetCreateInfo = {
        .attributes = ppSetAttribs,
        .attributeCount = 2,
        .renderStage = postRenderStage};

    VkuDescriptorSet ppDescriptorSet = vkuCreateDescriptorSet(&ppDescriptorSetCreateInfo);

    VkuVertexAttribute ppVertexAttribs[] = {};
    VkuVertexLayout ppVertexLayout = {.attributeCount = 0, .attributes = ppVertexAttribs, .vertexSize = 0 * sizeof(float)};
    VkuPipelineCreateInfo ppPipelineInfo = {
        .vertexShaderSpirV = vkuReadFile("./shader/post_vert.spv", &ppPipelineInfo.vertexShaderLength),
        .fragmentShaderSpirV = vkuReadFile("./shader/post_frag.spv", &ppPipelineInfo.fragmentShaderLength),
        .vertexLayout = ppVertexLayout,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .descriptorSet = ppDescriptorSet,
        .depthTestWrite = VK_FALSE,
        .depthTestEnable = VK_FALSE,
        .depthCompareMode = VK_COMPARE_OP_LESS,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .renderStage = postRenderStage};

    VkuPipeline ppPipeline = vkuCreatePipeline(context, &ppPipelineInfo);

    free(ppPipelineInfo.fragmentShaderSpirV);
    free(ppPipelineInfo.vertexShaderSpirV);

    printf("VRAM: %.2f MB!\n", (float)vkuMemoryMamgerGetAllocatedMemorySize(memoryManager) / 1000000.0f);

    double previousTime = 0.0;
    double currentTime = 0.0;
    int frameCount = 0;

    while (!vkuWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - previousTime >= 1.0) {
            char title[256];
            snprintf(title, sizeof(title), "VkuTest - %d FPS", frameCount);
            glfwSetWindowTitle(window->glfwWindow, title);
            frameCount = 0;
            previousTime = currentTime;
        }

        VkuFrame frame = vkuPresenterBeginFrame(presenter);
        vkuFrameBeginRenderStage(frame, renderStage);
        vkuFrameBindPipeline(frame, pipeline);

        ubo ubo;
        glm_perspective(glm_rad(60.0f), vkuWindowGetAspect(window), 0.1f, 10.0f, ubo.projection);
        glm_lookat((vec3){2.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.001, 1.0f, 0.0f}, ubo.view);
        glm_mat4_identity(ubo.model);
        glm_rotate(ubo.model, 1.0f, (vec3){(float)sin(glfwGetTime()), (float)cos(glfwGetTime()), (float)sin(glfwGetTime())});
        vkuFrameUpdateUniformBuffer(frame, uniformBuffer, (void *)&ubo);
        vkuFrameDrawVertexBuffer(frame, vertexBuffer, 36);

        vkuFrameFinishRenderStage(frame, renderStage);

        vkuFrameBeginRenderStage(frame, postRenderStage);
        vkuFrameBindPipeline(frame, ppPipeline);
        vkuFrameDrawVoid(frame, 3);
        vkuFrameFinishRenderStage(frame, postRenderStage);

        vkuPresenterSubmitFrame(frame);
    }

    vkuDestroyPipeline(context, ppPipeline);
    vkuDestroyDescriptorSet(ppDescriptorSet);
    vkuDestroyRenderStage(postRenderStage);
    vkuDestroyPipeline(context, pipeline);
    vkuDestroyDescriptorSet(descriptorSet);
    vkuDestroyUniformBuffer(context, uniformBuffer);
    vkuDestroyTextureSampler(context, sampler);
    vkuDestroyTexture2DArray(context, texArray);
    vkuDestroyStaticRenderStage(renderStage);
    vkuEnqueueBufferDestruction(memoryManager, vertexBuffer);
    vkuDestroyBuffersInDestructionQueue(memoryManager);
    vkuDestroyPresenter(presenter);
    vkuDestroyContext(context);

    printf("Terminated!\n");
    return 0;
}