# 常见错误

## 如果调试时遇到transform函数处理 call指令时函数找不到的问题

这是unity代码裁剪引起的。你需要在主工程中显式带上类或者函数的使用，如RefTypes.cs所做的那样。

如果你是新使用huatuo的项目。建议将 [huatuo_trail](https://github.com/focus-creative-games/huatuo_trial)/Assets/Main/HuatuoLib 拷贝到你主工程，
然后再在RefTypes.cs里添加你要引用的类型。

## 凡是遇到FileLoadException：xxxxxx/StreamingAssets/HotFix.dll 错误的问题

这是il2cppd代码里面抛出的，原因是在编译之前没有替换huatuo源码中的libil2cpp目录

解决办法：

  1. 确定已经将了huatuo代码中的 libil2cpp 目录替换了对应的Editor 中的 libil2cpp 目录
  2. 删除Unity工程中的Library目录，重新构建即可

