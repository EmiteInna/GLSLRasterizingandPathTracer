# GLSLRasterizingandPathTracer
继续改存货，在原来的基础上写个path tracing的渲染器。

前置都是glfw+glad+glm+stbi_img+assimp（learnopengl上的标准配件）。

由于github上传有限制，框架和模型素材就没放进去了，只包含核心代码，具体参考learnOpenGL。

-

EMTR-简单的光栅化渲染器，没有任何光照和阴影，只有baseMap的采样，但保留了接口，可以继续写shader来实现光照和阴影（因为learnOpenGL很详细了，就没敲，毕竟本来是想做路径追踪，不需要这些）。

每个文件对应各自的类，用法参考代码中的klee部分（声明一个Model，在初始化函数里new出来，绑定材质，然后在渲染函数里调用Render即可

面向对象水平一般，各个角落充斥着ACM习惯，设计就看个乐呵吧。

-

EMTB-BVH-构建以及其可视化显示，本来不想单拉出来，但是debug了一年，所以就……。做了些小优化，主要针对SAH，虽然感觉能把SAH卡成线性时间复杂度的模型应该是不存在的。

为了把不同的模型都加载进来并且显示不同的材质，用了一个meshIndex来做mesh索引（也就是说一个小mesh对应一个材质）。因为只有一个material，所以不同材质的具体使用打算直接在shader里搞个switch case直接去采，也就是说每有一个小mesh就需要在shader里加一个Disney系统的全套变量，而不是基于顶点。

-

EMTP-路径追踪初版的修复版本，另外加了对独显的调用，速度飞升，修复了一些bug，没有加重要性采样，效果比较一般，可能还有别的bug。

