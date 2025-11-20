# KEngine

一个基于Hazel引擎架构的学习型游戏引擎项目，专注于渲染子系统的实现与理解。

## 项目简介

这个项目是我在学习游戏引擎的架构的时候做的，前半段与cherno教程一致，后半段开始学习opengl渲染，实现LearnOpenGL教程中的一些渲染技术

## 项目结构

KEngine/
├── KEngine/                 # 核心引擎代码
│  ├── src/                 # 引擎源码
│   ├── vendor/              # 第三方依赖库
├── Sandbox/                 # 示例与测试应用
│   ├── RendererLayer.cpp       # 渲染层实现
│   ├── Scenes/              # 渲染技术展示场景
└── vendor/                  # 构建工具

## 构建说明

### 前置要求

- Visual Studio 2022
- C++17

### 构建步骤

1. 克隆仓库以及子模块
2. 双击GenerationProjects.bat进行项目构建
3. 点开生成的sln进行生成

## 开发日志

- **2025-9-1**: 项目启动
- **2025-9-7**: 完成P13，Log系统实现
- **2025-9-10**: 完成P16，ImGui集成
- **2025-9-15**: 完成P30，渲染器架构设计
- **2025-9-20**: 完成P33，Command模式实现
- **2025-9-23**: 完成P36，DLL生成问题记录
- **2025-10-21**: 自主实现渲染层，暂停跟随教程
- **2025-11-20**: 项目暂停，转向ShaderToy学习

## 技术栈

- C++
- OpenGL
- ImGui

## 致谢

- [Cherno的Hazel引擎教程](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
- [主页 - LearnOpenGL CN](https://learnopengl-cn.github.io/)
- 以及各种博客网站
