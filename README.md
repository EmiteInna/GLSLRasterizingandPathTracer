# GLSLRasterizingandPathTracer
继续改存货，在原来的基础上写个path tracing的渲染器。

前置都是glfw+glad+glm+stbi_img+assimp（learnopengl上的标准配件）。

由于github上传有限制，框架和模型素材就没放进去了，只包含核心代码，具体参考learnOpenGL。

EMTR-最开始的光栅化渲染器，每个文件对应各自的类，用法参考代码中的klee部分（声明一个Model，在初始化函数里new出来，绑定材质，然后在渲染函数里调用Render即可）。没有任何光照和阴影，只有baseMap的采样，但保留了接口，可以继续写shader（之后可能会写……

EMTB-BVH-构建以及其可视化显示，本来不想单拉出来，但是debug了一年，而且做了些自己的小优化。

EMTB-BVP-路径追踪初版，出了点问题，暂时下线。
