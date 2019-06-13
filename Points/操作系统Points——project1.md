# 操作系统Points——project

## 实验目标

实验一的最终任务就是在threads/中跑make check的时候，27个test全部pass

## Mission1

**重新实现timer_sleep函数**

**思路**：将忙则等待机制换为唤醒机制，以减少对cpu资源的浪费。具体思路为：在timer_sleep（）函数中让该进程暂时阻塞（调用thread_block()），然后过了ticks个时间段后再把它加回到就绪队列中。通过改进timer_interrup()函数，使系统每次调用它的时候都检查一下这个进程是否已经等待了足够长的时间，如果时间不够长，则不管它，如果已经足够长了，则调用thread_unblock()函数将它唤醒到就绪队列中，至于如何才能得到一个线程被阻塞了多长时间，则需要给线程结构体加一个成员ticks_blocked来记录这个线程被阻塞时间，当这个线程被阻塞的时候，将ticks_blocked记录为需要等待的时间长度，并且每次中断的时候都将其减一，当它小到等于0的时候，把线程唤醒到就绪队列中。

**几个概念：**

忙则等待：当一个进程位于临界区内时，任何试图进入临界区的进程都必须在其代码中连续地循环，虽然在等待但是一直占用cpu资源

唤醒：当线程处于阻塞状态时，线程处于等待队列中，当满足唤醒条件后，将其唤醒并将其加入到就绪队列中，否则它一直处于等待状态

中断：中断分两种，一种是IO设备向cpu发出的中断的信息，另一种是cpu决定切换到另一个进程时（轮换时间片）发出的指令。Pintos的中断为第二种，默认每一个ticks调用一次时间中断，即每一个线程最多可以占据cpu一个ticks的时长，到时间后就必须释放cpu。

## timer_sleep函数：

**修改前**：系统最开始是使用忙则等待实现的，即线程不停地循环，直到时间片耗尽。timer_sleep是在ticks时间内， 如果线程处于running状态就不断把他扔到就绪队列不让它执行，线程依然不断在cpu就绪队列和running队列之间来回，占用了cpu资源

```
void
timer_sleep (int64_t ticks)
{
  int64_t start = timer_ticks ();//获取ticks的当前值返回
  ASSERT (intr_get_level () == INTR_ON);//断言（高级异常处理）start获取了起始时间， 断言必须可以被中断，不然会一直死循环下去
  while (timer_elapsed (start) < ticks)// 在ticks的时间内不断执行thread_yield
    thread_yield();
}
```

**部分涉及函数：**

1.timer_ticks()获取ticks的当前返回值

2. intr_get_level()返回了intr_level的值

3. intr_disable ()获取了当前的中断状态， 然后将当前中断状态改为不能被中断， 然后返回执行之前的中断状态。

4. timer_elapsed返回了当前时间距离then的时间间隔

5.thread_yield其实就是把当前线程扔到就绪队列里，然后重新schedule， 注意这里如果就绪队列为空的话当前线程会继续在cpu执行

6.thread_current()获取当前当前的线程的指针

**修改**：调用timer_sleep的时候直接把线程阻塞掉，然后给线程结构体加一个成员ticks_blocked来记录这个线程被阻塞时间， 然后利用操作系统自身的时钟中断，在timer_interrupt函数中加入对线程状态的检测， 每次检测将ticks_blocked减1, 如果减到0就唤醒这个线程，所以每次时间片轮转时都会调度timer_interrupt函数

**修改位置：**

(1). 在线程的结构体加上ticks_blocked成员用于记录被sleep的时间![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224525749.png)

(2). 在线程被创建（thread_create函数）的时候初始化ticks_blocked为0（t->ticks_blocked = 0;）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224546618.png)

(3). 修改时钟中断处理函数（timer_interrupt函数），加入线程sleep时间的检测， thread_foreach (blocked_thread_check, NULL);（thread_foreach就是对每个线程都执行blocked_thread_check这个函数）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224600520.png)

(4). 编写blocked_thread_check函数（编写在thread.c中）

```
/* 检查阻塞线程，每次时间片用完时，我们都需要将每个线程的ticks_blocked减1，如果该线程ticks_blocked小于0，则将其唤醒 */
void
blocked_thread_check (struct thread *t, void *aux UNUSED)
{
  if (t->status == THREAD_BLOCKED && t->ticks_blocked > 0)
  {
      t->ticks_blocked--;
      if (t->ticks_blocked == 0)//次数小于0，唤醒
      {
          thread_unblock(t);// 不运行，但准备运行 把线程丢到就绪队列里继续跑
      }
  }
}
```


(5). threa.h中添加void blocked_thread_check (struct thread *t, void *aux UNUSED);
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224622673.png)

**修改后：**

```
void
timer_sleep (int64_t ticks)
{
  if (ticks <= 0)  
  {
    return;
  }
  ASSERT (intr_get_level () == INTR_ON);
  enum intr_level old_level = intr_disable();//与下面第四句一起保证操作的原子性
  struct thread *current_thread = thread_current ();//thread_current返回当前线程起始指针位置
  current_thread->ticks_blocked = ticks;// 设置阻塞时间
  thread_block ();//等待触发事件
  intr_set_level (old_level); //与上面第四句一起保证操作的原子性
}
```

**部分涉及函数：**

1.thread_foreach(thread_action_func *func, void *aux)遍历当前就绪队列中的所有线程，并且对于每一个线程执行一次func操作（func是一个任意给定函数的指针，参数aux是想要传给这个函数的参数），所有就绪的线程都被保存在一个链表中，此函数是要遍历一遍链表（函数只能在中断关闭的时候调用）

2.thread_block()和thread_unblock第一个函数的作用是把当前占用cpu的线程阻塞掉，第二个函数作用是将已经被阻塞掉的线程唤醒到就绪队列中

3.timer_interrupt (struct intr_frame *args UNUSED) 函数，pintos在每次时间中断时（即每一个时间单位（ticks））调用一次这个函数

## 遇到的问题：

报错：

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224719270.png)

运行失败

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224730349.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0xUYW5nZXJpbmVf,size_16,color_FFFFFF,t_70)

解决方法

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224741415.png)

运行成功

运行结果：

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190603224755586.png)

