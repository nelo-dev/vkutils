![Logo](run/resources/icon_medium.png)
# VkUtils
## 🕶️ Overview
This project is my attempt at creating a Vulkan API abstraction for C, as there are not many libraries for the C devs out there. This abstraction simplifies Vulkan use very much without sacrificing performance. Its intended use is for simple or medium sized games or scientific applications.

## 📋 Features
- Abstraction of VkInstance, VkDevice, VmaAllocator, etc. into the **VkuContext**. It serves as the base for your program.
- The **VkuMemoryManager** uses the VMA from AMD to manage memory efficiently and create or destroy a **VkuBuffer**.
- The **VkuPresenter** creates an Window and can be used as a RenderTarget. It holds the VkSwapchainKHR handles syncronization while rendering. It also recreates all necessary resources on window resize.
- A **VkuRenderStage** is my abstraction of handling a VkRenderPass along with its VkFramebuffers and resources (Images & ImageViews). When MSAA is enabled render resources (not for resolve) are shared along multiple renderStages. A VkuRenderStage can be **static** (fixed Resolution) or **dynamic** (resolution and format scale with window).
- Easy creation of textures **VkuTexture2D** and texture arrays **VkuTexture2DArray** from pixel data (uint8_t*)
- Easy creation of descriptorSets (**VkuDescriptorSet**), currently supporting **VkuSampler** (alongside a texture or texture array) and a **VkuUniformBuffer**. It abstracts VkDescriptorSetLayout.
- Each renderStage can have multiple **VkuPipeline**'s. The pipeline takes in the shaders, **VkuVertexLayout** and many other relevant render options and abstracts VkPipelineLayout. 
- A frame is handled in a **VkuFrame**. It manages the cmdBuffer. In the frame renderStages, pipelines, etc. can be bound, uniformBuffer updated and drawCmds send. (All function have VKU wrapper functions)
- Full **C++ compatibility** (yes, C code must not be cpp compatible).

## ⬇️ Installation
Just include the **vkutils.c** & **vkutils.h** files from the src/ folder in your project. Remember to also compile the **vkutils.c** along the other files.

## Quick Start
To quickly get something draw into the window, a VkuContext and VkuPresenter have to be created. A VkuPresenter has to be created right after VkuContext, because it may reinitializes the Vulkan objects to work with the window. There are currently 3 modes for a VkuContext:
- VKU_CONTEXT_USAGE_BASIC will create basic Vulkan objects. In future it can be used for offscreen rendering
- VKU_CONTEXT_USAGE_PRESENTATION will not create Vulkan objects on VkuContext creation. When a presenter is created it will initialize them with the fitting requirements. Will support frame dependent compute.
- VKU_CONTEXT_USAGE_COMPUTE is not implement yet, but will optimize the VkuContext for GPGPU offscreen applications.
```c
VkuContextCreateInfo contextCreateInfo = {
    .enableValidation = VK_TRUE, //enables Vulkan Validation
    .applicationName = "VkuTest",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0), //your application version
    .usage = VKU_CONTEXT_USAGE_PRESENTATION};

VkuContext context = vkuCreateContext(&contextCreateInfo);

VkuPresenterCreateInfo presenterCreateInfo = {
    .context = context,
    .width = 1152,
    .height = 720,
    .windowTitle = "VkuTest",
    .windowIconPath = "./resources/icon.png",
    .presentMode = VK_PRESENT_MODE_FIFO_KHR, //VK_PRESENT_MODE_FIFO_KHR is like vsync else VK_PRESENT_MODE_FIFO_KHR is recommend (as fast a possible)
    .framesInFlight = 2, //procomputed frames by the cpu. Distributes cpu load evenly at the cost of latency
};

VkuPresenter presenter = vkuCreatePresenter(&presenterCreateInfo);

// Creation of other Vku objects and renderloop

vkuDestroyPresenter(presenter);
vkuDestroyContext(context);
```

## Examples
Examples can be found at the **run/** folder along some shaders. These examples demonstrate basic and advanced usage of **VkUtils**.

## ⚠️ Considerations & Warnings
- This library is not professional or excessively tested.
- Im not a native english speaker, so there can be spelling mistakes.
- This library is a by-product of making my own game and features can depend of the requirements of the game.
- Im not a full time dev. At the moment im 19 years old and cannot ensure consistend development and activity on this project along side university.

## 🏗️ Use in your Project
- **MIT License**
- A **mention** would be very nice. You can also use the icon of this project.
- If you want, you can message me so I can see what cool projects you are working on.
10,0*e^(-3988*(1/(298,15)-1/(x+273,15)))