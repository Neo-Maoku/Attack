# Attack
### 一、shellcode加载器

1. 攻击思路：利用线程栈空间存放shellcode代码，使用函数指针指向shellcode起始处并调用，这样可以不使用VirtualAlloc申请内存存放代码，尽可能少用被标记为危险的系统函数

2. 实现原理：
   - 通过python脚本把shellcode划分成函数需要调用的参数形式
   - 在vs代码中创建6个数组，这个数量是可以根据函数调用的参数动态调整的，假设函数参数为4，加上函数调用时会把返回地址和esp压入栈中，所以需要加上2个参数的长度就是6个参数为一组
   - 计算存放shellcode需要多少次函数回调，这次可以使用递归实现函数回调，到达最后一层时需要记录栈顶的地址（shellcode的起始地址）
   - 根据起始地址依次把shellcode截断的部分填充返回地址和esp的地址内容
   - 定义一个无返回值，无参数的指针函数指向shellcode开始处

3. 实现过程中遇到的问题与解决方法

   - 栈空间默认不可执行：修改项目属性，把DEP保护关闭


   - 当栈中有调用系统函数时会把shellcode的内容覆盖掉：加大函数递归的次数，使shellcode的数据放置更高的位置，这个填充的数据大小需要根据系统函数会用到的栈空间决定，一般可以给大点不超过栈限制大小即可

4. 待优化内容

   - 填充的数据脚本化


   - 添加x64版本


   - 添加反调试模块


   - 目前是选择参数执行躲避沙箱检测，添加无参数或key执行躲避沙箱功能

5. 实战经验

   - win defender会把样本放内置沙箱执行
   - 微步沙箱在检测到监听鼠标点击时，会去模拟鼠标点击事件以触发木马行为
