# Chapter17

该项目展示了如何使用 PLTHook 技术来获取网络请求相关信息

运行环境
=====
AndroidStudio3.2
NDK16~19
支持 `x86` `armeabi-v7a`

说明
====

运行项目后点击`开启 Socket Hook`按钮，然后点击`请求`按钮。

整个实现与[ThreadHook](https://github.com/AndroidAdvanceWithGeektime/Chapter06-plus)基本一样，大家可以参考之前的文档

实现差异
====
不过跟ThreadHook相比，整体实现还有略有差异，这一次我们增加了一个`hook_plt_method_all_lib`方法

```
int hook_plt_method_all_lib(const char* exclueLibname, const char* name, hook_func hook) {
  if (refresh_shared_libs()) {
    // Could not properly refresh the cache of shared library data
    return -1;
  }

  int failures = 0;

  for (auto const& lib : allSharedLibs()) {
      if (strcmp(lib.first.c_str(), exclueLibname) != 0) {
        failures += hook_plt_method(lib.first.c_str(), name, hook);
      }
  }

  return failures;
}
```

它的作用是不仅仅hook某个so，而是hook内存中的所有so。但是要排除掉方法本身定义的so，不然运行期间会出问题。