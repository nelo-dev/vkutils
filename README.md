![Logo](run/resources/icon_medium.png)
# VkUtils
## Overview
This project is my attempt at creating a Vulkan API abstraction for C, as there are not many libraries for the C devs out there. This abstraction simplifies Vulkan use very much without sacrificing performance. Its intended use is for simple or medium sized games or scientific applications.

## Features
- Abstraction of VkInstance, VkDevice, VmaAllocator, etc. into the **VkuContext**. It serves as the base for your program.
- The **VkuMemoryManager** uses the VMA from AMD to manage memory efficiently and create or destroy a **VkuBuffer**.
- The **VkuPresenter** creates an Window and can be used as a RenderTarget. It holds the VkSwapchainKHR handles syncronization while rendering. It also recreates all necessary resources on window resize.
- A **VkuRenderStage** is my abstraction of handling a VkRenderPass along with its VkFramebuffers and resources (Images & ImageViews). When MSAA is enabled render resources (not for resolve) are shared along multiple renderStages. A VkuRenderStage can be **static** (fixed Resolution) or **dynamic** (resolution and format scale with window).
- Easy creation of textures **VkuTexture2D** and texture arrays **VkuTexture2DArray** from pixel data (uint8_t*)
- Easy creation of descriptorSets (**VkuDescriptorSet**), currently supporting **VkuSampler** (alongside a texture or texture array) and a **VkuUniformBuffer**. It abstracts VkDescriptorSetLayout.
- Each renderStage can have multiple **VkuPipeline**'s. The pipeline takes in the shaders, **VkuVertexLayout** and many other relevant render options and abstracts VkPipelineLayout. 
- A frame is handled in a **VkuFrame**. It manages the cmdBuffer. In the frame renderStages, pipelines, etc. can be bound, uniformBuffer updated and drawCmds send. (All function have VKU wrapper functions)
- Full **C++ compatibility** (yes, C code must not be cpp compatible).

## Installation
Just include the **vkutils.c** & **vkutils.h** files from the src/ folder in your project. Remember to also compile the **vkutils.c** along the other files.

## Considerations & Warnings
- This library is not professional or excessively tested.
- Im not a native english speaker, so there can be spelling mistakes.
- This library is a by-product of making my own game and features can depend of the requirements of the game.
- Im not a full time dev. At the moment im 19 years old and cannot ensure consistend development and activity on this project along side university.

## Use in your Project
- **MIT License**
- A **mention** would be very nice. You can also use the icon of this project.
- If you want, you can message me so I can see what cool projects you are working on.