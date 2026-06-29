# 太初仙王录 (Immortal Sovereign: Path of Ascension)

一个基于 SFML 3 的 C++ 游戏项目，采用严格分层的 MVC 架构。

本文档同步自项目架构规范，是所有协作者必须遵守的分层职责与交互契约。代码实现规范另见仓库说明结尾的「代码规范」一节。

## 开发者

- 天才小彩虹 (RainbowSpringCat)
- 轻轻咬腿（沛总）

## 环境要求

| 依赖 | 版本要求 |
| --- | --- |
| C++ 标准 | C++17 |
| CMake | ≥ 3.22 |
| SFML | 3.x（Graphics / Window / System 组件） |
| 构建工具 | Ninja（推荐） |
| 编译器 | 支持 C++17 的 Clang / GCC / MSVC |

## 构建步骤

```bash
# 配置
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# 构建
cmake --build build

# 运行
./build/Immortal-Sovereign-Path-of-Ascension
```

> 注意：`build/` 为本地构建产物，已在 `.gitignore` 中忽略，请勿提交。每位协作者在本地各自构建即可。

---

# 架构规范

本节定义项目统一的 MVC 分层职责与各层之间的交互/时序契约，所有模块实现都必须遵守。

## 各层职责

- **Director（导演）**：拥有窗口、主循环及引擎相关基础设施（主线程派发器、资源管理器、音频播放器），负责创建对象、绑定依赖、驱动生命周期与场景切换。只做"创建与编排"，不承载业务逻辑。因持有窗口与事件轮询，允许依赖 SFML。
- **Controller（控制器）**：业务逻辑的协调核心。持有其所辖 Model、Service、View 的非拥有引用并向它们下达指令；实现 View 的 `userEvent` 接口、Model 的 observer 接口、Service 的回调接口，作为这三类上报的统一接收方；通过抽象接口（如 `IArchiveStore`、`IAudioPlayer`）使用持久化与音频能力。不持有 Director；需要切换场景或退出时，经 Delegate 的流程接口 `IGameFlow` 向 Director 上报**语义事件**（由 Director 决定流转），不指定目标场景。不依赖 SFML、不直接做渲染。Controller 之间不互相持有。
- **Scene（场景）**：实现 `IScene` 接口的场景封装类（如 `LoginScene`/`MainScene`），自持本场景的 View / Controller / Model 并在构造时完成依赖绑定，向 Director 暴露 `handleEvent / update / draw`。是 Director 同层的编排构件，由 Director 持有，允许依赖 SFML；只向下持有本场景对象，不被 Controller / View 认识。
- **View（视图）**：UI 展示。不持有 Controller，用户操作通过 `userEvent`（委托接口 / `std::function`）向上上报。View 不主动重绘，只在每帧被主循环调用 `draw()` 时按当前待显示状态绘制；待显示数据由 Controller 通过 setter 推入。允许依赖 SFML；可引用资源管理器按 key 取资源。
- **Model（模型）**：领域数据与纯数值业务逻辑（如关卡公式递推），是运行期数据真相（由各 Model 分别持有，如玩家数据 / 设置 / 关卡）。不直接操作 View、不持有 Controller；数据变化只通过 observer 接口通知。只存资源的逻辑 key（纯字符串），不持有 SFML 资源。可依赖 Config、Data。
- **Service（服务）**：I/O 与系统能力封装（如存档读写、加解密）。不持有 View、不直接改 View、不依赖 SFML；文件 I/O 一律用标准库。异步 I/O 在工作线程完成并经主线程派发器切回主线程后，通过回调接口通知 Controller。可依赖 Config、Data、Delegate 与主线程派发器。
- **Config（配置）**：集中定义静态基准与可调常量。两类：数值基准（基础属性、成长系数等）；模块静态参数（存档相对文件名、存档版本号、字体/资源/BGM 的默认文件名与出厂默认值等）。处于依赖最底层，只提供静态值，不读取外部文件、不参与运行期业务判断。
- **Data（数据结构）**：无行为的被动数据结构与共享枚举（POD/struct）。可被各层共享；不含 observer、不含业务方法，不反向依赖任何上层。
- **Delegate（委托接口）**：集中存放所有跨层抽象接口，用于解耦而不让一方持有另一方的具体类型。包含两类：① 下层向上通信的回调接口（View 的 `userEvent`、Model 的 observer、Service 的回调）；② 上层依赖下层实现的服务抽象（持久化 `IArchiveStore`、音频 `IAudioPlayer`）；③ Controller 向 Director 上报的流程接口 `IGameFlow`（语义事件如 `onLoginSucceeded`/`onQuitRequested`，由 Director 决定场景流转）。具体类型实现这些接口，依赖方只持有接口指针。

## 引擎相关基础设施

由 Director 持有、允许依赖 SFML 的基础设施，不属于业务上层；其源码置于 `Director/` 下或独立 `Infra/` 目录：

- **主线程派发器**：线程安全任务队列。Service 把异步完成的回调投入其中，主循环每帧排空并在主线程执行。是"异步回调切回主线程"的统一载体。
- **资源管理器（ResourceManager）**：加载并按 key 缓存/共享字体、图片与音效缓冲等 SFML 资源（`sf::Font` / `sf::Texture` / `sf::SoundBuffer`）。Director 持有，View 持其引用按 key 取资源。是引擎相关基础设施而非 Service。
- **音频播放器（AudioPlayer）**：负责 BGM 与音效的播放/暂停/换曲/音量控制。BGM 用流式 `sf::Music`，音效用 `sf::SoundBuffer` + `sf::Sound`。依赖 SFML，Director 持有；Controller 经抽象接口 `IAudioPlayer`（以资源 key、音量等纯数据为参数）指挥，自身不依赖 SFML。

## 层间依赖方向

允许的依赖方向为"上层依赖下层"，下层严禁依赖上层的具体类型，只能经 Delegate 接口反向通信。

```
Director（依赖 SFML：窗口/主循环/派发器/资源管理器/音频播放器） → 当前 Scene(IScene) → 该场景的 { Controller, View, Model }
    Scene      → 该场景的 Controller / View / Model（向下持有并装配），实现 IScene 供 Director 驱动   [允许依赖 SFML]
    Controller → Model, Service, View, Config, Data, Delegate(含 IArchiveStore / IAudioPlayer / IGameFlow)
    Model      → Config, Data, Delegate(observer 接口)
    Service    → Config, Data, Delegate(回调接口 / IArchiveStore), 主线程派发器     [纯标准库，不依赖 SFML]
    View       → Data, Delegate(userEvent 接口), 资源管理器, SFML
引擎基础设施（依赖 SFML，Director 持有）：资源管理器（被 View 引用）、音频播放器（被 Controller 经 IAudioPlayer 指挥）、派发器
最底层： Config、Data、Delegate （不依赖任何上层）
```

- 允许依赖 SFML 的只有 **View、场景类（Scene）、Director 及 Director 持有的引擎基础设施（资源管理器、音频播放器）**；Model / Controller / Config / Data / Service 一律不得依赖 SFML。
- Model / Service / View 之间不互相依赖。
- 下层向上通知只引用 Delegate 接口，绝不 `#include` 或持有 Controller / Director 的具体类型。

## 所有权约定

- Director 持有当前场景类（`std::unique_ptr<IScene>`，同一时刻只持一个）及基础设施（派发器、资源管理器、音频播放器）的所有权；场景类持有本场景的 View / Controller / Model（值成员或 `std::unique_ptr`）。
- Controller 对 View / Model / Service 持有非拥有引用，不负责其生命周期。
- Service 仅持有主线程派发器的引用，不拥有它，故不违反"下层不持上层"。

## 交互/时序规则

1. **依赖绑定**：Director 创建当前场景类，并把引擎基础设施（资源管理器引用、`IGameFlow` 流程接口、`IArchiveStore` / `IAudioPlayer` 实现）注入场景；场景类在构造时完成**场景内**装配——把 Controller 注册为 View 的 `userEvent` 委托、Model 的 observer、Service 的回调接收方，把 `IGameFlow` / `IArchiveStore` / `IAudioPlayer` 交给 Controller，把资源管理器引用交给 View。
2. **用户输入流**：主循环把原始事件分发给当前场景的 View → View 判断命中并以 `userEvent` 上报 → Controller 处理。View 不持有 Controller、不调用其业务方法。
3. **数据变化流**：Controller 改 Model → Model 通过 observer 接口通知（注册者为 Controller）→ Controller 通过 View 的 setter 推入待显示数据。Model 不感知也不直接操作 View。
4. **异步 I/O 流**：Controller 经 `IArchiveStore` 发起请求 → Service 工作线程完成 I/O → 把回调封装为任务投入主线程派发器 → 主循环排空派发器在主线程执行回调 → 经回调接口通知 Controller → Controller 更新 Model 或 View。`load` / `save` 均为异步，结果经回调返回，不同步返回；回调唯一接收方是 Controller（不是 Director）。
5. **配置使用**：Config 仅提供数值基准与静态参数供初始化、公式计算与各模块取用；运行期的数值递推与业务判断由 Model / Controller 完成，不在 Config 内做判断。
6. **场景流转流**：Controller 处理用户意图后，经 Delegate 的流程接口 `IGameFlow` 向 Director 上报**语义事件**（如 `onLoginSucceeded`/`onQuitRequested`），**不指定目标场景**；由 **Director 决定**该事件对应去哪个场景或退出，并在**帧末安全执行**（销毁旧场景类、用工厂按 `SceneId` 建新场景类）。Controller 不持有 Director、不认识 `SceneId`、不自行切换场景。

> **回调/流程接口的实现约定**：上报接收方统一以**内部适配器**实现这些接口——私有嵌套类持外层引用并把调用转发给外层的私有处理函数——以规避多重继承、且不把回调方法暴露到对象的公有接口。适用于 Controller（接收 View 的 `userEvent`、Model 的 observer、Service 回调）与 Director（接收 Controller 的 `IGameFlow`）。

## 运行期模型：主循环与渲染驱动

SFML 是"主循环 + 每帧重绘"的保留式渲染，与"观察者按需通知"的 MVC 需明确协同：

- **窗口归属**：`sf::RenderWindow` 由 Director 拥有。窗口事件轮询与渲染只在拥有窗口的主线程进行。
- **主循环（Director 驱动）**，每帧顺序固定：
  1. 轮询输入：`pollEvent` → 交给当前 `Scene.handleEvent` → 场景内 View 判命中并以 `userEvent` 上报 Controller。
  2. 排空主线程派发器：执行异步完成投入的回调（"切回主线程"的落地点）。
  3. 更新：当前 `Scene.update(dt)` 推进时间相关逻辑（打字机、动画、计时）。
  4. 渲染：`clear` → 当前 `Scene.draw(window)` → `display`。
  5. 帧末处理：执行本帧经 `IGameFlow` 记录的待切换场景 / 退出请求（安全销毁旧场景、装配新场景）。
- **观察者与重绘协同**：observer 只负责"改 View 的待显示状态"，主循环只负责"每帧把当前状态画出来"。Model 变化 → Controller 收到通知 → setter 进 View；下一帧主循环照 View 当前状态重绘。View 自身从不主动触发重绘。

## 全局配置（字体 / 图片 / BGM / 用户设置）

- **静态默认参数**（字体文件名、图片资源包文件名/相对路径、BGM 默认文件名与出厂默认音量等）由 Config 以具名常量提供。
- **资源的实际加载与持有**由资源管理器完成（依赖 SFML，Director 持有），View 按 key 取用。
- **BGM / 音效的播放与控制**由音频播放器负责，Controller 经 `IAudioPlayer` 指挥（播放 key、设音量等）。
- **用户可调设置**（音量、是否静音、分辨率等）是可变运行期状态，存入设置 Model，并持久化到**独立的设备级设置文件**（与账号存档分开）。设置在启动时、加载任何账号存档之前即读取生效（主菜单即可用）。Config 只提供出厂默认值，不存用户改动。

## 关卡与数值生成

- 关卡强度等可无限延伸的数据不落地为外部配置文件，由 Model 层以纯公式在运行期递推生成。
- 递推规则示例：第 n 关属性 = 基础值 × 成长系数^(n-1)，基础值与成长系数（如 `kStageGrowthRate`）为 Config 提供的具名常量。
- 公式递推属于 Model 的业务逻辑；Config 只提供基准常量，不做递推与判断。

## 存档与持久化

- **运行期数据真相是各 Model**；本地 JSON 文件仅作持久化，不引入数据库。
- **持久化接口** `IArchiveStore`（Delegate 层）：定义异步 `load` / `save` 等操作。Controller 只依赖该接口，不关心底层是本地文件还是远程服务器；`load` 结果经回调（切回主线程）返回，非同步返回。
- **本地实现** `ArchiveService`（Service 层）：用标准库 `std::fstream` 读写；JSON 序列化（单头文件 `nlohmann/json`，置于 `third_party/`，纯标准库、不依赖 SFML）；负责加解密、版本号校验与迁移；异步完成后经主线程派发器回调 Controller。不依赖 SFML、不持有 View。
- **账号与存档**：一个账号对应唯一 ID，ID 绑定一个账号存档。本地阶段 ID 由本地生成（如本地 profile UUID）；真正的账号体系待服务器阶段接入。账号存档与上面的设备级设置文件是两份独立数据。
- **存档路径**：相对文件名与版本号由 Config 提供；平台相关的存档目录（win/安卓/ios/mac 不同）由 Service 在运行期向系统查询获取。
- **加密定位（权衡）**：客户端本地加密仅为防作弊的轻量混淆（劝退随手改档），密钥随客户端分发不能视作真正安全；真正的防作弊依赖后续服务器权威。密钥不以明文具名常量硬编码，此权衡在实现说明中标注。
- **演进路径**：后续"上服务器做完整保存"时，新增一个远程 `IArchiveStore` 实现替换本地实现即可，Controller / Model 不变。

## 场景与生命周期

> **Scene 与 MVC 的关系**：Scene 不属于经典 MVC 三件套，而是叠在 MVC 之上、与之正交的**场景/状态编排层**（游戏圈常见的 Scene/Screen/GameState 模式）。MVC 描述"一个场景内部"的数据/展示/逻辑分工；Scene 描述"场景之间"的组装与生命周期。它让 Director 保持瘦（只管窗口/主循环/工厂/流转决策），并使"帧末整体替换当前场景"有一个明确的可替换单元（`unique_ptr<IScene>`）。其合理性前提：① 项目本就多场景（登录/主游戏/战斗/面板）；② Scene 只做装配与转发，不写业务、不渲染、不持领域数据，Controller/View 也不认识它。一旦 Scene 开始承载业务或被下层反向依赖，即视为越界。后续若需叠层 UI（如主游戏上盖暂停菜单、底层不销毁），再由单一 `unique_ptr<IScene>` 演进为场景栈（push/pop），`IScene` 接口基本不动。

- 每个场景（登录 / 主游戏 / 战斗 / 面板等）由一个实现 `IScene` 接口的**场景类**（如 `LoginScene`/`MainScene`）封装，**自持本场景的 View / Controller / Model 并在构造时完成依赖绑定**；`IScene` 暴露 `handleEvent / update / draw` 供主循环统一驱动。
- 场景类是 **Director 同层的编排构件**：由 Director 拥有（`std::unique_ptr<IScene>`，同一时刻只持当前场景一个），允许依赖 SFML（事件/窗口/字体），源码置于 `Scene/` 目录；只向下持有 View/Controller/Model，Controller/View 不认识场景类。
- 场景的**创建/销毁/切换决策仍归 Director**：用工厂 `createScene(SceneId)` 按标识建场景；`SceneId` 是 Director 内部概念，不暴露给 Controller。
- **切换时机为帧末**：切换/退出请求在事件回调中经 `IGameFlow` 产生，Director 只记录待处理目标，待本帧"轮询 → 更新 → 渲染"结束后再真正销毁旧场景、装配新场景——避免在某场景的回调执行中销毁其自身导致崩溃。

## 目录组织约定

游戏模块置于 `src/game/`，按层划分子目录：

```
src/game/
├── Config/        # 配置层：静态基准与可调常量（数值基准、存档文件名/版本、资源默认值等具名常量，不读外部文件）
├── Data/          # 数据结构层：无行为的纯数据结构与共享枚举（如 PlayerArchiveData、StageTypes）
├── Delegate/      # 委托层：所有跨层接口（userEvent、observer、Service 回调、IArchiveStore、IAudioPlayer、IGameFlow 流程接口）
├── Model/         # 数据层：领域模型与纯数值逻辑（含关卡公式递推），禁止依赖引擎（不得 #include SFML）
├── Service/       # 服务层：I/O 与系统能力（存档读写/加解密，纯标准库，异步后回调 Controller，不依赖 SFML）
├── View/          # 视图层：UI 展示，允许依赖 SFML；每帧被动 draw，不主动重绘
├── Controller/    # 控制层：业务逻辑协调核心，实现 Delegate 接口作为上报统一接收方
├── Scene/         # 场景层：实现 IScene，装配并自持单个场景的 View/Controller/Model（依赖 SFML，Director 持有）
├── Director/      # 导演层：拥有窗口/主循环及引擎基础设施；创建场景、决定流转、驱动主循环
└── Infra/         # 引擎相关基础设施（可选独立目录）：主线程派发器、资源管理器、音频播放器（依赖 SFML，Director 持有）
```

### 模块内文件归类（子文件夹约定）

每层目录下，按文件是否跨模块复用再分：

- **只服务单一模块/场景的文件** → 放进该层下以模块名命名的子文件夹，如 `Config/Login/`、`View/Login/`、`Controller/Login/`。
- **会被多个模块复用的文件** → 直接放在该层根目录，如 `Config/WindowConfig.h`、`Data/AuthState.h`、`Model/AccountModel.h`。

示例（登录模块）：

```
Config/   WindowConfig.h  AccountConfig.h  ResourceConfig.h   Login/LoginUiConfig.h
Data/     AuthState.h                                         Login/{LoginRequest,LoginResult}.h
Delegate/ IAccountObserver.h  IGameFlow.h                     Login/ILoginViewDelegate.h
Model/    AccountModel.{h,cpp}
View/                                                         Login/LoginView.{h,cpp}
Controller/                                                   Login/LoginController.{h,cpp}
Scene/    IScene.h  LoginScene.{h,cpp}
```

### 头文件包含路径

- 以 `src/game` 为包含根（CMake `target_include_directories` 指向它），`#include` 一律使用相对该根的路径，如 `#include "Config/Login/LoginUiConfig.h"`，**不使用 `../` 上跳相对路径**，避免移动文件后断链。

依赖约束要点：

- **Model 层禁止依赖引擎**：纯 C++ 数据/逻辑，不得 `#include <SFML/...>`、不持有 SFML 类型。
- **允许依赖 SFML 的只有 View、场景类（Scene）、Director 及其持有的引擎基础设施（资源管理器、音频播放器）**；Service 仅用标准库做文件 I/O，不操作窗口与渲染。
- **窗口与渲染线程约束**：窗口轮询与渲染必须在拥有窗口的主线程；Service 异步 I/O 完成后须经主线程派发器切回主线程再回调 Controller。
- **下层不持上层具体类型**：Model / Service / View 经 Delegate 接口向上通信，不 `#include` Controller / Director。
- **关卡/数值公式递推生成**：无限延伸的数据不落地为配置文件，由 Model 以纯公式运行期递推；Config 只提供基准常量。
- **持久化与音频经接口隔离**：Controller 依赖 `IArchiveStore` / `IAudioPlayer` 接口，本地实现与未来远程/替换实现可互换。
- **设置与存档分离**：用户设置存设备级设置文件，账号存档存账号文件，两者独立持久化。
- **Config 处于依赖最底层**，不反向依赖任何层。
- **Data 与 Model 的界线**：Data 是被动结构（无 observer、无业务方法），可被各层共享；Model 是有状态、带 observer 与业务方法的领域对象，可依赖 Data，Data 不反向依赖 Model。

---

## 代码规范

除架构分层外，项目还有一套强制的 C++ 代码实现规范（命名规范、文件注释风格、禁止事项、const 正确性、函数/类规模控制、内存管理、防御性编程等）。提交代码前请确保符合规范，并对照执行改动后的 `git diff` 自检流程。
