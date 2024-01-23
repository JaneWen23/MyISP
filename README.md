# MyISP

## Features:

算法级: 
- 使用函数式编程, 抽象化算法设计, 独立性好, 可复用,可组合; 接口统一, 提高开发效率;
- 目前完成的算法包括: 可灵活配置滤波核的滤波操作, 小波变换(二代方法); Star-Tetrix变换; CCM等.

模块级: 
- 合理封装算法, 作为算法与pipeline的衔接, 也解除算法与pipeline的耦合, 降低维护难度.

管线级: 
- 支持配置DAG (有向无环图) 定义 pipeline 拓扑结构, 支持带延迟的输入, 快速仿真任何ISP pipeline
- 使用哈希表系统化、层次化管理算法配置参数, 同时适配可变的pipeline结构.
- 配置内容最小化, 尽量避免人为的输入错误



## 使用说明:

如何配置 pipeline:

***(目前未做 “通过 config 文件定义 pipeline 拓扑结构” 的功能, 做好后更新)***

首先我们思考一下 pipeline 是由哪些东西定义的:

- pipeline 是由很多模块组成的, 它们之间的依赖关系可以用一个有向无环图表示, 因此需要这样一个图, 来定义**一帧图像的各个模块的依赖关系**;

- 考虑到可能有的模块需要历史帧的输入, 仅有一个有向无环图就无法表示了, 所以在此基础上, 还需要定义**历史帧的依赖关系**; 

- 由于算法的参列表顺序是定死的, 如果一个模块需要多个输入图像(一般是从其他模块输出的), 还需**要根据算法参列表顺序**定义该模块的**输入图像的接收顺序**.

所以需要从以上三个维度来定义 pipeline. 并不是所有的 pipeline 都有复杂的结构, 简单的 pipeline 不需要写全所有三个维度来定义. 下面举例说明:

### 最简单的级联 pipeline:

```mermaid
graph LR;
    DUMMY0-->DUMMY1;
    DUMMY1-->DUMMY2;
```

用代码的语言描述其拓扑结构:
```cpp
// 只需要描述第一个维度
int n = 3; // number of nodes
Graph_t graph(n);

graph[0] = {DUMMY0, {DUMMY1}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1
graph[1] = {DUMMY1, {DUMMY2}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY2
graph[2] = {DUMMY2, {}}; 
```

用此拓扑结构初始化 pipeline 并打印信息:

```cpp
Pipeline myPipe(graph, true); // true 表示要打印信息
```
terminal 输出:

```
pipe:
DUMMY0:   needs no input;   delivers output to: DUMMY1, 
DUMMY1:   takes input(s) from: DUMMY0,   delivers output to: DUMMY2, 
DUMMY2:   takes input(s) from: DUMMY1,   dose not deliver output; 
```

### 纯有向无环图 pipeline:

```mermaid
graph LR;
    DUMMY0-->DUMMY1;
    DUMMY0-->DUMMY2;
    DUMMY1-->DUMMY3;
    DUMMY2-->DUMMY3;
    DUMMY3-->DUMMY4;
    DUMMY3-->DUMMY5;
    DUMMY4-->DUMMY6;
    DUMMY5-->DUMMY6;
    DUMMY7-->DUMMY6;
    DUMMY6-->DUMMY8;
```

用代码的语言描述其拓扑结构:

```cpp
// 第一个维度:
int n = 9; // number of nodes
Graph_t graph(n);

graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
graph[1] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
graph[2] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
graph[3] = {DUMMY3, {DUMMY4, DUMMY5}}; // and so on ...
graph[4] = {DUMMY4, {DUMMY6}};
graph[5] = {DUMMY5, {DUMMY6}};
graph[6] = {DUMMY6, {DUMMY8}};
graph[7] = {DUMMY7, {DUMMY6}}; 
graph[8] = {DUMMY8, {}};

// 第二个维度不需要, 因为不涉及历史帧

// 第三个维度:
Orders_t orders;
orders.push_back({DUMMY3, {{DUMMY1}, {DUMMY2}}}); // mind the syntax! {DUMMY1} is actually {DUMMY1, 0}, the 0 is default and therefore omitted.
orders.push_back({DUMMY6, {{DUMMY4}, {DUMMY5}, {DUMMY7}}});
```

用此拓扑结构初始化 pipeline 并打印信息:

```cpp
Pipeline myPipe(graph, orders, true); // true 表示要打印信息
```
terminal 输出:
```
pipe:
DUMMY7:   needs no input;   delivers output to: DUMMY6, 
DUMMY0:   needs no input;   delivers output to: DUMMY1, DUMMY2, 
DUMMY2:   takes input(s) from: DUMMY0,   delivers output to: DUMMY3, 
DUMMY1:   takes input(s) from: DUMMY0,   delivers output to: DUMMY3, 
DUMMY3:   takes input(s) from: DUMMY1, DUMMY2,   delivers output to: DUMMY4, DUMMY5, 
DUMMY5:   takes input(s) from: DUMMY3,   delivers output to: DUMMY6, 
DUMMY4:   takes input(s) from: DUMMY3,   delivers output to: DUMMY6, 
DUMMY6:   takes input(s) from: DUMMY4, DUMMY5, DUMMY7,   delivers output to: DUMMY8, 
DUMMY8:   takes input(s) from: DUMMY6,   dose not deliver output; 
```

### 有向无环图带历史帧的 pipeline

```mermaid
  flowchart LR;
  DUMMY8_at_t-->DUMMY8_at_t+1
  DUMMY8_at_t+1-->DUMMY8_at_t+2
  DUMMY1_at_t-->DUMMY2_at_t+1
  DUMMY1_at_t+1-->DUMMY2_at_t+2
  DUMMY1_at_t-->DUMMY2_at_t+2

    subgraph time = t
    direction LR
    DUMMY0_at_t-->DUMMY1_at_t;
    DUMMY0_at_t-->DUMMY2_at_t;
    DUMMY1_at_t-->DUMMY3_at_t;
    DUMMY2_at_t-->DUMMY3_at_t;
    DUMMY3_at_t-->DUMMY4_at_t;
    DUMMY3_at_t-->DUMMY5_at_t;
    DUMMY4_at_t-->DUMMY6_at_t;
    DUMMY5_at_t-->DUMMY6_at_t;
    DUMMY7_at_t-->DUMMY6_at_t;
    DUMMY6_at_t-->DUMMY8_at_t;
    end

    subgraph time = t + 1
    direction LR
    DUMMY0_at_t+1-->DUMMY1_at_t+1;
    DUMMY0_at_t+1-->DUMMY2_at_t+1;
    DUMMY1_at_t+1-->DUMMY3_at_t+1;
    DUMMY2_at_t+1-->DUMMY3_at_t+1;
    DUMMY3_at_t+1-->DUMMY4_at_t+1;
    DUMMY3_at_t+1-->DUMMY5_at_t+1;
    DUMMY4_at_t+1-->DUMMY6_at_t+1;
    DUMMY5_at_t+1-->DUMMY6_at_t+1;
    DUMMY7_at_t+1-->DUMMY6_at_t+1;
    DUMMY6_at_t+1-->DUMMY8_at_t+1;
    end

    subgraph time = t + 2
    direction LR
    DUMMY0_at_t+2-->DUMMY1_at_t+2;
    DUMMY0_at_t+2-->DUMMY2_at_t+2;
    DUMMY1_at_t+2-->DUMMY3_at_t+2;
    DUMMY2_at_t+2-->DUMMY3_at_t+2;
    DUMMY3_at_t+2-->DUMMY4_at_t+2;
    DUMMY3_at_t+2-->DUMMY5_at_t+2;
    DUMMY4_at_t+2-->DUMMY6_at_t+2;
    DUMMY5_at_t+2-->DUMMY6_at_t+2;
    DUMMY7_at_t+2-->DUMMY6_at_t+2;
    DUMMY6_at_t+2-->DUMMY8_at_t+2;
    end

```

用代码的语言描述其拓扑结构:

```cpp
// 第一个维度:
int n = 9; // number of nodes
Graph_t graph(n);

graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
graph[1] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
graph[2] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
graph[3] = {DUMMY3, {DUMMY4, DUMMY5}}; // and so on ...
graph[4] = {DUMMY4, {DUMMY6}};
graph[5] = {DUMMY5, {DUMMY6}};
graph[6] = {DUMMY6, {DUMMY8}};
graph[7] = {DUMMY7, {DUMMY6}}; 
graph[8] = {DUMMY8, {}};

// 第二个维度:
DelayGraph_t delayGraph;
delayGraph.push_back({DUMMY8, {{DUMMY8, 1}}});
delayGraph.push_back({DUMMY1, {{DUMMY2, 1}, {DUMMY2, 2}}});

// 第三个维度:
Orders_t orders;
orders.push_back({DUMMY3, {{DUMMY1 }, {DUMMY2 }}}); // mind the syntax! {DUMMY1} is actually {DUMMY1, 0}, the 0 is default and therefore omitted.
orders.push_back({DUMMY6, {{DUMMY4 }, {DUMMY5 }, {DUMMY7 }}});
orders.push_back({DUMMY8, {{DUMMY6 }, {DUMMY8, 1}}});
orders.push_back({DUMMY2, {{DUMMY0 }, {DUMMY1, 1}, {DUMMY1, 2}}});
```

用此拓扑结构初始化 pipeline 并打印信息:

```cpp
Pipeline myPipe(graph, delayGraph, orders, true); // true 表示要打印信息
```
terminal 输出:
```
pipe:
DUMMY7:   needs no input;   delivers output to: DUMMY6, 
DUMMY0:   needs no input;   delivers output to: DUMMY1, DUMMY2, 
DUMMY2:   takes input(s) from: DUMMY0, DUMMY1(last frame), DUMMY1(last 2nd frame),   delivers output to: DUMMY3, 
DUMMY1:   takes input(s) from: DUMMY0,   delivers output to: DUMMY3, DUMMY2(next frame), DUMMY2(next 2nd frame), 
DUMMY3:   takes input(s) from: DUMMY1, DUMMY2,   delivers output to: DUMMY4, DUMMY5, 
DUMMY5:   takes input(s) from: DUMMY3,   delivers output to: DUMMY6, 
DUMMY4:   takes input(s) from: DUMMY3,   delivers output to: DUMMY6, 
DUMMY6:   takes input(s) from: DUMMY4, DUMMY5, DUMMY7,   delivers output to: DUMMY8, 
DUMMY8:   takes input(s) from: DUMMY6, DUMMY8(last frame),   delivers output to: DUMMY8(next frame), 
```
