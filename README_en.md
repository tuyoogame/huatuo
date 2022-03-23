#huatuo

[![license](http://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

<br/>

![icon](docs/images/logo.png)

<br/>

Huatuo is a near-perfect C# hot update solution with complete features, zero cost, high performance, and low memory.

huatuo provides a very complete cross-platform CLR runtime, which can be efficiently executed in **AOT+interpreter** mixed mode not only on Android platform, but also on IOS, Consoles and other platforms that restrict JIT.

## Features

- Features complete. Nearly complete implementation of the [ECMA-335 specification](https://www.ecma-international.org/publications-and-standards/standards/ecma-335/), except for the features below "Limitations and Notes" are supported.
- Zero learning and usage costs. huatuo is a full CLR runtime, hot update code works seamlessly with AOT code. There is no need to write any special code, no code generation, and no special restrictions. Script classes are in the same runtime as AOT classes, even code like reflection, multi-threading (volatile, ThreadStatic, Task, async) works fine.
- Execute efficiently. Implemented an extremely efficient register interpreter that outperformed other hot update schemes by a large margin by all metrics. [Performance test report](docs/benchmark.md)
- Memory efficient. The classes defined in the hot update script occupy the same memory space as ordinary C# classes, which is far superior to other hot update solutions. [Memory usage report](docs/memory.md)
- Native support to fix some AOT codes. Does not add any additional development and running overhead.

## working principle

Inspired by mono's [Hybrid mode execution](https://developpaper.com/new-net-interpreter-mono-has-arrived/) technology, huatuo extends Unity's [il2cpp](https://docs. unity3d.com/Manual/IL2CPP.html) runtime, transform it from pure AOT runtime to "AOT + Interpreter" hybrid runtime, and completely support hot update dll from the bottom.

![icon](docs/images/architecture.png)

More specifically, huatuo does the following work:

- Implemented an efficient metadata (dll) parsing library
- Modified the metadata management module of il2cpp to realize the dynamic registration of metadata
- Implemented a compiler from an IL instruction set to a custom register instruction set
- Implemented an efficient register interpreter
- Provide a large number of instinct functions additionally to improve the performance of the interpreter
- Provide hotfix AOT support (in progress)

## Documentation

- [wiki](docs/home.md)
- Quick Start [Documentation](docs/start_up.md), [Video](docs/start_up.md)
- [FAQ](docs/FAQ.md)
- [Example Project](https://github.com/focus-creative-games/huatuo_demo)
- [Know the column] (https://www.zhihu.com/column/c_1489549396035870720)
- Support and Contact
  - QQ group: 651188171 (huatuo c# hot update development exchange group)
  - Email: taojingjian#gmail.com

## Completion status

- Almost all the features required by the specification are implemented. Except for AOT generics that cannot be solved generically
- Completed unit tests for most instructions. Only a few instructions that are difficult or impossible to construct, such as calli and initblk, do not have corresponding unit tests.
- Completing various syntax tests against the c# specification.
- Can load [luban](https://github.com/focus-creative-games/luban) configuration efficiently
- Can run less complex games like [2048](https://github.com/dgkanatsios/2048) normally as a pure interpreter

Our team is working hard to test and fix huatuo, **expect to run a large game project smoothly in 2022.4 (yes, next month)**.

## Limitations and Notes

**Features not included in the restrictions are all supported by huatuo**. Please stop asking if huatuo supports a certain feature.

- Currently only version 2020.3.7f is supported. It is expected that the full range of versions 2018-2022 will be supported after May.
- Unable to create instances of **hot update non-enumeration value types** of ordinary AOT generics (**delegate, Nullable, arrays (including multi-dimensional) are not limited, hot update generics are also completely unlimited**) instance of the type. For example, List&lt;HotUpdateValueType&gt; is not supported but List&lt;int&gt;, List&lt;HotUpdateClass&gt; and List&lt;HotUpdateEnum&gt; are supported. For specific reasons, see [AOT generic limitation and principle introduction](docs/generic_limit.md). This follow-up version will basically solve it.
- Currently, async tasks that return **custom value types** are not supported. Primitive value types such as int and enumeration and class types are not restricted for the same reason as above. This limitation will be resolved in subsequent releases.
- **Note to use link.xml or code reference to avoid Unity cutting code. Avoid the error that the function can be called during the development period, but the function cannot be found after the release. We will provide default templates in the future.
- BeginInvoke and EndInvoke of delegate are not supported. It just doesn't feel necessary to do it.
- Due to the limitation of Unity's resource management mechanism (the script uuid has been determined during packaging, and the GUID corresponding to the hot update script cannot be found), the hot update MonoBehaviour cannot be successfully deserialized in the scene or prefab. But it can be mounted by code go.AddComponent. It is also possible to load through some custom script proxy. I hope that the unity official or a third party will come up with a more convenient solution.
- Incremental gc is not supported yet. Due to the tight time, it is too late to carefully deal with the memory barrier details of incremental gc. This issue will be resolved soon.
- Debugging is temporarily not supported. But can print the error stack (only accurate to the function).

## RoadMap

Although huatuo is related to il2cpp, most of the core code is independent of il2cpp and can be easily ported (expected one month) to other CLR platforms that do not support AOT+Interpreter. No matter how the version of Unity changes, even if il2cpp is abandoned and .net 6+ is used, huatuo will continue to follow up and stably provide cross-platform CLR hot update service until one day when .net officially supports AOT+Interpreter, huatuo will complete its historical mission.

- Continue to fix bugs to make a medium and large game work properly (2022.4)
- Instruction optimization, the number of instructions after compilation is reduced to 1/4-1/2, and the performance of basic instructions and most object model instructions is improved by 100%-300%. (2022.5 preview version)
- Support all series versions after unity 2018 (2022.5)
- Support hotfix bug in AOT part (2022.6)
- Remove most common AOT generic class restrictions (2022.7)
- Support for extern functions (2022.8)
- Support incremental gc. (2022.10)
- Other memory and efficiency optimizations, and code refactoring
- **===godot engine support ===**(2022.12)

## license

Huatuo is licensed under the [MIT](https://github.com/focus-creative-games/luban/blob/main/LICENSE.TXT) license