通常情况系，程序发生段错误时系统会发送SIGSEGV信号给程序，缺省处理是退出函数。我们可以使用 signal(SIGSEGV, &your_function);函数来接管SIGSEGV信号的处理，程序在发生段错误后，自动调用我们准备好的函数，从而在那个函数里来获取当前函数调用栈。

举例如下：

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

void dump(int signo)
{
 void *buffer[30] = {0};
 size_t size;
 char **strings = NULL;
 size_t i = 0;

 size = backtrace(buffer, 30);
 fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
 strings = backtrace_symbols(buffer, size);
 if (strings == NULL)
 {
  perror("backtrace_symbols.");
  exit(EXIT_FAILURE);
 }
 
 for (i = 0; i < size; i++)
 {
  fprintf(stdout, "%s\n", strings[i]);
 }
 free(strings);
 strings = NULL;
 exit(0);
}

void func_c()
{
 *((volatile char *)0x0) = 0x9999;
}

void func_b()
{
 func_c();
}

void func_a()
{
 func_b();
}

int main(int argc, const char *argv[])
{
 if (signal(SIGSEGV, dump) == SIG_ERR)
  perror("can't catch SIGSEGV");
 func_a();
 return 0;
}

编译程序：
gcc -g -rdynamic test.c -o test; ./test
输出如下：

Obtained6stackframes.nm
./backstrace_debug(dump+0x45)[0x80487c9]
[0x468400]
./backstrace_debug(func_b+0x8)[0x804888c]
./backstrace_debug(func_a+0x8)[0x8048896]
./backstrace_debug(main+0x33)[0x80488cb]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x129113]

接着：
objdump -d test > test.s
在test.s中搜索804888c如下：

8048884 <func_b>:
8048884: 55          push %ebp
8048885: 89 e5      mov %esp, %ebp
8048887: e8 eb ff ff ff      call 8048877 <func_c>
804888c: 5d            pop %ebp
804888d: c3            ret

其中80488c时调用（call 8048877）C函数后的地址，虽然并没有直接定位到C函数，通过汇编代码， 基本可以推出是C函数出问题了（pop指令不会导致段错误的）。
我们也可以通过addr2line来查看

addr2line 0x804888c -e backstrace_debug -f 

输出：

func_b
/home/astrol/c/backstrace_debug.c:57