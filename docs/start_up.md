# huatuo

当前仅支持 2020.3.7f版本(后续很快会支持2018以后的全系列版本)。

可参考 [示例项目](https://github.com/focus-creative-games/huatuo_trial)。

## 使用

以下为了方便起见，发布win下pc版本。其他平台同理。

### 安装

- 安装 2020.3.7f 版本,注意要选中win64下的il2cpp模块（如果你发布其他平台，则选择相应平台的il2cpp模块）。
- 用huatuo项目的libil2cpp目录替换Editor安装目录下的 Editor/Data/il2cpp/libil2cpp 目录。

至此即完成huatuo安装，后续打包出的app就能支持c#热更新。是不是简单到有点匪夷所思？

### 项目的准备工作

- 使用 Unity的 Assembly def 创建一个专门的 HotFix 模块（你也可以使用创建第三方工程的方式，只不过麻烦了点）。
- 配置HotFix模块
  - 根据需求设置正确的Assembly Definition Reference。起码得引用主工程吧。
  - Platforms 选项下，取消AnyPlatform，接着只选中 Editor和随便一个**不导出**的平台（如XboxOne）。不能只有Editor，因为Unity会把它当作纯Editor模板，不允许加载MonoBehaviour。
- 将示例项目Assets/Main/HuatuoLib 以及Assets/link.xml 拷贝到 你项目中（非必须，纯粹是为了防止unity裁剪，如果你已经正确设置了link.xml，则可忽略这步）

### 代码中使用

- 在HotFix模块中创建第一个热更新脚本，类似如下

```c#
public class App
{
    public static int Main()
    {
        Debug.Log("hello,huatuo");
        return 0;
    }
}
```

- 主工程中，使用标准反射函数加载Hotfix.dll,以示例 LoadDll.cs 为例

```c#

public class LoadDll : MonoBehaviour
{
    void Start()
    {
        LoadGameDll();
        RunMain();
    }

    private System.Reflection.Assembly gameAss;

    private void LoadGameDll()
    {
#if UNITY_EDITOR
        string gameDll = Application.dataPath + "/../Library/ScriptAssemblies/HotFix.dll";
        // 使用File.ReadAllBytes是为了避免Editor下gameDll文件被占用导致后续编译后无法覆盖
        gameAss = System.Reflection.Assembly.Load(File.ReadAllBytes(gameDll));
#else
        string gameDll = Application.streamingAssetsPath + "/HotFix.dll";
        gameAss = System.Reflection.Assembly.LoadFile(gameDll);
#endif
    }

    public void RunMain()
    {
        if (gameAss == null)
        {
            UnityEngine.Debug.LogError("dll未加载");
            return;
        }
        var appType = gameAss.GetType("App");
        var mainMethod = appType.GetMethod("Main");
        mainMethod.Invoke(null, null);
    }
}
```

- Build & Run
  - 将 HotFix.dll拷到StreamingAssets下
  - 发布选项设置
    - Scripting Backend 选择 il2cpp backend
    - Api Compatible level 选择 .NET 4.x
    - 取消 use incremental GC
  - 发布
  - 进入场景后，应该能看到日志 "hello,huatuo"

- 测试热更新

修改HotFix模块中的 App::Main代码。改成

```c#
public class App
{
    public static int Main()
    {
        Debug.Log("hello,world");
        return 0;
    }
}
```

将编译后的Hotfix.dll 复制替换发布目标目录下的 StreamingAssets下的Hotfix.dll，重新运行程序。
你应该会看到打印出 "hello,world"。

至此完成。
