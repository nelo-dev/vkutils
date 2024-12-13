![Logo](run/resources/icon_medium.png)
# VkUtils

## Overview
This project is my attempt at creating a Vulkan API abstraction for C, as there are not many libraries for the C devs out there. This abstraction simplifies Vulkan use very much without sacrificing performance. Its intendet use is for simple or medium sized Games or scientific applications.

## Features
- Abstraction of VkInstance, VkDevice, VmaAllocator, etc into VkuContext
- The VkuPresenter creates an Window and can be used as a RenderTarget. It holds the VkSwapchainKHR handles syncronization while rendering. It also recreates all necessary resources on window resize.
- A VkuRenderStage is my abstraction of handling a VkRenderPass along with its VkFramebuffers and resources. When MSAA is enabled render resources (not for resolve) are shared along multiple renderStages.
