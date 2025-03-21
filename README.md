GZHU grade 15 Principles of Compiler Design experiments personal backup
====

This repository is created for personal backup.  
Highly NOT recommand using codes in this repo to submit homework.  

experiment 1  
---
- 编码实现将各类编程语言 （不仅限指导书中提供的 C 语言样例） 源程序翻译成对应的二元组 TOKEN 序列，并能检查一定的词法错误，要求在实现的代码上做好详细注释。  
- 编码实现的功能包含对关键字、 带下划线的标识符、 字符串常量、 注释的识别，10 或 16 进制的整型常量、带指数的双精度常量的识别，以及词法错误处理(选做)。  
- (Optional) 上传源程序代码图片，识别图中有效字符代码，并按照基础实验要求完成对代码中不同类型单词的识别和输出。输入： 包含源代码的图片（图片格式不限，但必须包含基础实验要求中待识别的单词对象）。输出：同基础实验要求一样。

1. 了解 C 语言样例中待分析的词法规范  
关键字 例如：main if else int return void 等  
操作符和标点 例如：= + * >= ; ( ) 等  
标识符(ID) —— 字符、数字和下划线组成的字符串，以字符开头  
英文大小写敏感， 例如 if 是关键字， 但是 IF 是一个标识符；ben 和 Ben 是两个不同的标识符  
数值常量(NUM)  
整型常量：可以是 10 进制或者 16 进制。16 进制整数必须以 0x 或者 0X 开头，后面跟一串 16 进制字符  
双精度常量：由一串数字，一个小数点，后面跟一串数字（或者没有）组成。因此.12 不是一个有效的双精度数，但是 0.12 和 12.都是。双精度数也可以是带指数的，如：12.2E+2。对于这样一个科学计数， 10 进制的小数点是必须的， 指数的符号是必须的，E 可以大写或者小写，和上面一样。12E+2 不是有效的，而 12.E+2 是有效的。尾数和指数上一开始的 0 是允许的  
字符串常量(STRING) —— 一串被双引号围起来的字符。字符串可以包含除了换行符和双引号以外的所有字符。一个字符串不能够被切分成多行  
空白符 —— 空白符 （例如空格、 制表符、 换行符） 作为标号间的分隔符，可以被忽略掉。关键字和标识符必须被空白符或者一个既不是关键字也不是标识符的词法单元隔开。ifintthis 是一个单独的标识符，而不是三个关键字。if(23this 被作为 4 个 token 识别出来  
注释 —— 一个单行注释由//开始直到行末。多行注释由/\*开始，结束于\*/，不允许嵌套。注释中允许所有字符（除了结束这个注释的\*/和开始一个嵌套注释的/\*）  
2. 各种单词符号对应的种别码（可自行扩展）  
```csv
# word,id
main, 1
int, 2
char, 3
if, 4
else, 5
for, 6
while, 7
return, 8
void, 9
id, 10
", 11
', 12
num, 20
=, 21
+, 22
-, 23
*, 24
/, 25
(, 26
), 27
[, 28
], 29
{, 30
}, 31
,, 32
:, 33
;, 34
>, 35
<, 36
>=, 37
<=, 38
==, 39
!=, 40
string, 50

# id 101+ for dynamic id and others
```

3、 C 语言子集的词法分析程序功能
输入：C 语言源程序字符串。  
输出：二元组（syn,token 或 value）构成的序列（不限于二元信息，可输出更多元信息，种别码和单词自身值不可或缺。  
其中：syn 为单词种别码；token 为存放的单词自身字符串；value 为数值常量。  
检测词法错误：给出有意义的错误信息和错误发生的行号。例如：
字符@并非 C 程序中的合法符号，若这个字符在注释和字符串常量以外地方出现，则需要提示一个词法错误。  
字符串常量表示结束的右双引号在一行结束时都未出现，例如
“scanner。  
例如：对源程序“if x>9 x=2\*x+1/3; ”的源文件，经过词法分析后输出如下序列：  
(4,if) (10,x) (35,>) (20,9) (21,=) (20,2) (24,*) (22,+) (20,1) (25,/)(20,3) (34,;)……

experiment 2  
---
- 将已构建的 LL(1)语法分析表进行编码存储  
- 编码实现查表分析的功能，该功能包含自左向右扫描输入串，依据当前输入 指针所指向的字符和栈顶元素进行查表，通过模拟进栈、出栈的操作来实现 分析过程  
- 输出内容包含三部分，按顺序分别是: LL(1)语法分析表、分析过程表以及分析结果
- (Optional) 编写一个 LL(1)语法分析器，不仅能依据输入的文法自动生成相应的LL(1)分析表，并实现对输入串的语法分析。输入:一个由四元组表示的文法、一个待识别的字符串。输出: 自动生成的 LL(1)分析表、分析过程表、分析结果  

1、 了解确定型自上而下语法分析方法(例如 LL(1)分析法)的使用  
前提条件——无左递归无回溯 设有文法 G\[E\]:  
E→E+T|T  
T→T\*F|F  
F→(E)|i  
消除左递归后的文法 G′\[E\]为:   
E→TE′  
E′→+TE′|ε  
T→FT′   
T′→\*FT′|ε  
F→(E)|i  
2、掌握由 FIRST 集和 FOLLOW 集构造语法分析表的规则  
依据文法 G\[E\]的所有 FIRST 集和 FOLLOW 集构造的 LL(1)分析表如下所示:   
(依据不同的文法将构建得到不同的 LL(1)分析表)   

|     | i        | +        | *        | (        | )        | #        |
|-----|----------|----------|----------|----------|----------|----------|
| E   | E → TE′  |          | E → TE′  |          |          |          |
| E′  | E′ → +TE′|          |          | E′ → ε   | E′ → ε   |          |
| T   | T → FT′  |          | T → FT′  |          |          |          |
| T′  | T′ → ε   | T′ → *FT′|          | T′ → ε   | T′ → ε   |          |
| F   | F → i    |          |          | F → (E)  |          |          | 		

3、LL(1)语法分析程序功能  
已知文法 G\[E\]  
输入:由文法 G\[E\]终结符构成的字符串。  
输出:  
- LL(1)分析表(要求表格形式)  
- 语法分析过程(要求表格形式)  
- 分析结果(即判断输入的字符串是否是该文法 G\[E\]的句子)。  
其输出分析过程形式如下表结构:(不同的输入串会产生不同分析过程)  

| 步骤  | 分析栈    | 当前输入 a  | 剩余输入串   | 所用产生式   |
|------|----------|------------|------------|-------------|
| 1    | #E       | I          | i+i#       | E → TE′     |
| 2    | #E’T     | I          | i+i#       | T → FT′     |
| 3    | #E’T’F   | I          | i+i#       | F → i       |
| 4    | #E’T’i   | I          | i+i#       |             |
| 5    | #E’T’    | +          | +i#        | T′ → ε      |
| 6    | #E′      | +          | +i#        | E′ → +TE′   |
| 7    | #E’T+    | +          | +i#        |             |
| 8    | #E’T     | I          | i#         | T → FT′     |
| 9    | #E’T’F   | I          | i#         | F → i       |
| 10   | #E’T’i   | I          | i#         |             |
| 11   | #E’T’    | #          | #          | T′ → ε      |
| 12   | #E′      | #          | #          | E′ → ε      |
| 13   | #        | #          | #          | 分析成功     |
