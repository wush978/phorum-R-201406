---
title       : Large Scale Learning in R
subtitle    : 
author      : Wush Wu
job         : Taiwan R User Group
framework   : io2012        # {io2012, html5slides, shower, dzslides, ...}
highlighter : highlight.js  # {highlight.js, prettify, highlight}
hitheme     : tomorrow       # 
widgets     : [mathjax]            # {mathjax, quiz, bootstrap}
mode        : selfcontained # {standalone, draft}
license: by-nc-sa
logo: Taiwan-R-logo.png
--- &vcenternobg .large




<img src="assets/img/MLDMMonday.jpg" class="grayscale fit100" />

Taiwan R User Group

MLDMMonday: 每周一分享资料相关议题

主题包含但不限于：

R 套件使用

机器学习和统计模型

--- &vcenter .large

很多人都在诟病R 无法处理大量的数据

--- &vcenter .large

但是只要用对R 的包

R 是可以处理大量的数据

--- &vcenter .large

今天我想跟大家分享

运用R 建立商用推荐引擎的心得

主要是靠着`Rcpp`和`pbdMPI`等包

打造可扩放性的学习模组

--- &vcenter .large

实际的数据和课本上的数据是不一样的

--- &twocolvcenter

*** =left

## 实际的数据

- 乱、充满错误
- 不停的变动
- 不知道怎样才算好
    - 只能不断精益求精

*** =right

## 课本的数据

- 干净
- 静止
- 需要复杂的算法

--- &vcenter .large

今天从这样的数据开始

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Sun May 25 00:30:10 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> is_click </TH> <TH> show_time </TH> <TH> client_ip </TH> <TH> adid </TH>  </TR>
  <TR> <TD align="right"> 1 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:06:52 </TD> <TD> 114.44.x.x </TD> <TD> 133594 </TD> </TR>
  <TR> <TD align="right"> 2 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:04:48 </TD> <TD> 1.162.x.x </TD> <TD> 134811 </TD> </TR>
  <TR> <TD align="right"> 3 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:03:05 </TD> <TD> 101.13.x.x </TD> <TD> 131990 </TD> </TR>
  <TR> <TD align="right"> 4 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:05:04 </TD> <TD> 24.84.x.x </TD> <TD> 134689 </TD> </TR>
  <TR> <TD align="right"> 5 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:03:26 </TD> <TD> 140.109.x.x </TD> <TD> 126437 </TD> </TR>
  <TR> <TD align="right"> 6 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:04:28 </TD> <TD> 61.231.x.x </TD> <TD> 131389 </TD> </TR>
   </TABLE>

<br/>
以“预测点击发生的机率”为例

--- &twocol .large

## 问题的建模

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Sun May 25 00:30:10 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> is_click </TH> <TH> show_time </TH> <TH> client_ip </TH> <TH> adid </TH>  </TR>
  <TR> <TD align="right"> 1 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:06:52 </TD> <TD> 114.44.x.x </TD> <TD> 133594 </TD> </TR>
  <TR> <TD align="right"> 2 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:04:48 </TD> <TD> 1.162.x.x </TD> <TD> 134811 </TD> </TR>
  <TR> <TD align="right"> 3 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:03:05 </TD> <TD> 101.13.x.x </TD> <TD> 131990 </TD> </TR>
  <TR> <TD align="right"> 4 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:05:04 </TD> <TD> 24.84.x.x </TD> <TD> 134689 </TD> </TR>
  <TR> <TD align="right"> 5 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:03:26 </TD> <TD> 140.109.x.x </TD> <TD> 126437 </TD> </TR>
  <TR> <TD align="right"> 6 </TD> <TD> FALSE </TD> <TD> 2014/05/17 04:04:28 </TD> <TD> 61.231.x.x </TD> <TD> 131389 </TD> </TR>
   </TABLE>

<h2>$$\Downarrow$$</h2>
<h2>$$P(y|x) = \frac{1}{1 + e^{-yw^Tx}}$$</h2>

--- &vcenter .large

$$w^Tx$$

--- &vcenter .large

iris


```r
head(model.matrix(Species ~ ., iris))
```

```
  (Intercept) Sepal.Length Sepal.Width Petal.Length Petal.Width
1           1          5.1         3.5          1.4         0.2
2           1          4.9         3.0          1.4         0.2
3           1          4.7         3.2          1.3         0.2
4           1          4.6         3.1          1.5         0.2
5           1          5.0         3.6          1.4         0.2
6           1          5.4         3.9          1.7         0.4
```


--- &vcenter .large

实际的资料

有 $10^{9+}$ 笔资料

有 $10^{3+}$ 笔广告

每种类别变量可能到 $10^{3}$ 类别

--- &vcenter .large

`model.matrix`的后果：

至少需要 $10^9 \times (10^3...)$ 个位元

也就是 $116.415$ GB的记忆体

--- &vcenter .large

开始抱怨了!

### R 不能处理大量的数据

### R 会吃很多记忆体

### 使用R 跑不动，使用其他工具就跑得动

--- &vcenter .large

请使用和其他工具一样的资料结构:

这种状况，应该使用`稀疏矩阵`

--- &twocolvcenter

*** =left

## 稀疏矩阵

- 只储存非0的资讯


```r
n <- 1
m1 <- matrix(0, 10^n, 10^n);m1[1, 4] <- 1
c(m1)
library(Matrix)
m2 <- Matrix(0, 10^n, 10^n, sparse=TRUE)
m2[1,4] <- 1
str(m2)
```


*** =right


```
  [1] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 [48] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 [95] 0 0 0 0 0 0
```

```
Formal class 'dgCMatrix' [package "Matrix"] with 6 slots
  ..@ i       : int 0
  ..@ p       : int [1:11] 0 0 0 0 1 1 1 1 1 1 ...
  ..@ Dim     : int [1:2] 10 10
  ..@ Dimnames:List of 2
  .. ..$ : NULL
  .. ..$ : NULL
  ..@ x       : num 1
  ..@ factors : list()
```


--- &vcenter .large

在我手上的资料上

大概可以省下 $10^2 \sim 10^3$ 倍记忆体

而且可以大幅度加快运算效能!

--- &vcenter .large

如果`m1`, `m2`是 $10^4 \times 10^4$ 的矩阵:


```
      test replications elapsed relative user.self sys.self user.child sys.child
1 m1 %*% r          100  21.690      868    47.727    0.217          0         0
2 m2 %*% r          100   0.025        1     0.021    0.003          0         0
```


--- &vcenter .large

可以利用`Rcpp`将C的实作暴露到R中

可以利用`Rcpp`做记忆体的重复使用

--- &vcenter .large





```
                test replications elapsed relative user.self sys.self user.child sys.child
1           m2 %*% r          100   0.023     7.67     0.021    0.002          0         0
2 XTv(m2, r, retval)          100   0.003     1.00     0.002    0.000          0         0
```


在现代的Rcpp架构下

将C++函数放到R中变得很简单

我们只需要专注在算法上


```cpp
#include <Rcpp.h>
using namespace Rcpp;
// [[Rcpp::export]]
SEXP XTv(S4 m, NumericVector v) {
  //...
}
```


--- &vcenter .large

再透过`pbdMPI`开发分散式矩阵乘法

利用更多CPU和更多记忆体提升效能

--- &vcenter .large

$$\left(\begin{array}{c}X_1 \\ X_2\end{array}\right) v = \left(\begin{array}{c}X_1 v \\ X_2 v\end{array}\right)$$

$$\left(v_1 , v_2\right) \left(\begin{array}{c}X_1 \\ X_2\end{array}\right) = v_1 X_1 + v_2 X_2$$

--- &twocolvcenter .large

***=left

## MPI

### 记忆体足够的话，较快

### 没有容错

***=right

## Hadoop

### 慢，要大量机器才有效果

### 有容错

--- &vcenter .large

`pbdMPI`是[pbdR(Programming with Big Data in R)](http://r-pbd.org/)专案的套件之一

<img src="assets/img/pbdMPI.png" class="fit50" />

--- &twocolvcenter .large

*** =left

## pbdMPI的优点

### 好安装
### 好开发

*** =right


```r
library(pbdMPI)
.rank <- comm.rank()
filename <- sprintf("%d.csv", .rank)
data <- read.csv(filename)
target <- reduce(sum(data$value), op="sum")
finalize()
```


--- &twocolvcenter .large

*** =left

## 最佳化算法

### 迭代次数少
### 不用计算Hessian矩阵
### 已有很棒的实作[LIBLINEAR](http://www.csie.ntu.edu.tw/~cjlin/liblinear/)

*** =right

<h3><a href="http://dx.doi.org/10.1145/1273496.1273567">Lin et al. 2007 Trust Region Newton Methods for Large-scale Logistic Regression</a></h3>

--- &vcenter .large

为了加强效能

并且能够更方便的更改模型

我们自己包LIBLINEAR到R中

--- &vcenter .large

## LIBLINEAR 原始码中的 `tron.h`


```cpp
class TRON
{
public:
  TRON(const function *fun_obj, double eps = 0.1, int max_iter = 1000);
  ~TRON();

  void tron(double *w);
	void set_print_string(void (*i_print) (const char *buf));

private:
	//...
};
```


- TRON是最佳化的核心算法的实作
- TRON没有牵涉到资料，甚至没有牵涉到Loss Function
- `function`提供了一个界面

--- &vcenter

## 界面：`function`


```cpp
class function
{
public:
  virtual double fun(double *w) = 0 ;
  virtual void grad(double *w, double *g) = 0 ;
	virtual void Hv(double *s, double *Hs) = 0 ;

	virtual int get_nr_variable(void) = 0 ;
	virtual ~function(void){}
};
```


- `fun`代表objective function
- `grad`是`fun`的gradient
- `Hv`是`fun`的hessian乘上一个向量

--- &twocolvcenter .large

## 利用`Rcpp Modules`把`function`暴露到R中

*** =left

### 实作Rfunction


```cpp
class Rfunction : public ::function {
  Rcpp::Function _fun, _grad, _Hv;
  Rcpp::NumericVector _w, _s;
  
  Rfunction(SEXP fun, SEXP grad, SEXP Hv, int n) 
  : _fun(fun), _grad(grad), _Hv(Hv), _w(n), _s(n) { }
  
  //...
};

SEXP tron//...
```


*** =right

将Rfunction暴露出来


```cpp
RCPP_MODULE(HsTrust) {  
  class_<Rfunction>("HsTrust")
	.constructor<SEXP, SEXP, SEXP, int>()
	.property("n", &Rfunction::get_nr_variable, 
    "Number of parameters")
	.method("tron", &tron)
	.method("tron_with_begin", 
    &tron_with_begin)
	;	
}
```


--- &twocolvcenter .large

*** =left

实作的结果可包装成[套件](https://bitbucket.org/wush978/largescalelogisticregression/src/4daf9c5bba5cd0e4f35afd813866e6da72ca92bb/?at=hstrust)



```r
library(devtools)
install_bitbucket(
  repo="largescalelogisticregression", 
  username="wush978", ref="hstrust")
```


*** =right

使用


```r
library(HsTrust)

beta <- 1
fun <- function(w) 
  sum((w-beta)^4)

grad <- function(w)
  4 * (w-beta)^3

Hs <- function(w, s) 
  12 * (w-beta)^2 * s

obj <- new(HsTrust, fun, grad, Hs, 1)
print(r <- obj$tron(1e-4, TRUE))
```


--- &vcenter


```r
library(HsTrust)
# ...
print(r <- obj$tron(1e-4, TRUE))
```



```
iter  1 act 8.025e-01 pre 6.667e-01 delta 4.186e-01 f 1.000e+00 |g| 4.000e+00 CG   1
iter  2 act 1.585e-01 pre 1.317e-01 delta 4.186e-01 f 1.975e-01 |g| 1.185e+00 CG   1
iter  3 act 3.131e-02 pre 2.601e-02 delta 4.186e-01 f 3.902e-02 |g| 3.512e-01 CG   1
iter  4 act 6.185e-03 pre 5.138e-03 delta 4.186e-01 f 7.707e-03 |g| 1.040e-01 CG   1
iter  5 act 1.222e-03 pre 1.015e-03 delta 4.186e-01 f 1.522e-03 |g| 3.083e-02 CG   1
iter  6 act 2.413e-04 pre 2.005e-04 delta 4.186e-01 f 3.007e-04 |g| 9.135e-03 CG   1
iter  7 act 4.767e-05 pre 3.960e-05 delta 4.186e-01 f 5.940e-05 |g| 2.707e-03 CG   1
iter  8 act 9.416e-06 pre 7.823e-06 delta 4.186e-01 f 1.173e-05 |g| 8.019e-04 CG   1
[1] 0.961
```



--- &vcenter .large

结合`pbdMPI`和`Rcpp Modules`

让TRON呼叫分散式的运算系统

SPMD架构，无master，一次资料交换


```r
objective_function <- function(w) {
  logger(sum(w))
  regularization <- sum((w - prior)^2) / 2
  lik <- sum(C * log(1 + exp(- y.value * Xv(X, w))))
  lik.all <- allreduce(x = lik, x.buffer = buffer.0, op = "sum")
  return(regularization + lik.all[1])
}

hs <- new(HsTrust, objective_function, ...)
```


--- &vcenter .large

SPMD架构

<img src="assets/img/SPMD.png" class="fit100"/>


--- &vcenter .large

`objective_function`非常易于修改

所以我们能将注意力专注于模型上


```r
objective_function <- function(w) {
  logger(sum(w))
  regularization <- sum((w - prior)^2) / 2
  lik <- sum(C * log(1 + exp(- y.value * Xv(X, w))))
  lik.all <- allreduce(x = lik, x.buffer = buffer.0, op = "sum")
  return(regularization + lik.all[1])
}
```


--- &vcenter .large

终于跨过资料量的门槛了...

该看看资料了！

--- .dark .segue

## 资料越大，结果就会越好吗？

---

<img src="assets/img/datasize.png" class="fit100"/>

--- .dark .segue

## 不同的模型对预测会有影响吗？

--- &vcenter .large

因子的组合

<img src="assets/img/factors.png" class="fit100" />

--- &vcenter .large

实验结果

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Sun May 25 00:30:10 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> auc </TH> <TH> Regularization </TH> <TH> FeatureSet </TH> <TH> DayEffect </TH>  </TR>
  <TR> <TD align="right"> 1 </TD> <TD align="right"> 1.0394 </TD> <TD align="right">     1 </TD> <TD> A </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 2 </TD> <TD align="right"> 1.0233 </TD> <TD align="right">    10 </TD> <TD> A </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 3 </TD> <TD align="right"> 1.0181 </TD> <TD align="right">    20 </TD> <TD> A </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 4 </TD> <TD align="right"> 1.0326 </TD> <TD align="right">     1 </TD> <TD> A </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 5 </TD> <TD align="right"> 1.0196 </TD> <TD align="right">    10 </TD> <TD> A </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 6 </TD> <TD align="right"> 1.0125 </TD> <TD align="right">    20 </TD> <TD> A </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 7 </TD> <TD align="right"> 1.0174 </TD> <TD align="right">     1 </TD> <TD> A </TD> <TD> 20:00:00 </TD> </TR>
  <TR> <TD align="right"> 8 </TD> <TD align="right"> 1.0057 </TD> <TD align="right">    10 </TD> <TD> A </TD> <TD> 20:00:00 </TD> </TR>
  <TR> <TD align="right"> 9 </TD> <TD align="right"> 1.0000 </TD> <TD align="right">    20 </TD> <TD> A </TD> <TD> 20:00:00 </TD> </TR>
  <TR> <TD align="right"> 10 </TD> <TD align="right"> 1.0436 </TD> <TD align="right">     1 </TD> <TD> B </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 11 </TD> <TD align="right"> 1.0277 </TD> <TD align="right">    10 </TD> <TD> B </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 12 </TD> <TD align="right"> 1.0215 </TD> <TD align="right">    20 </TD> <TD> B </TD> <TD> 09:00:00 </TD> </TR>
  <TR> <TD align="right"> 13 </TD> <TD align="right"> 1.0406 </TD> <TD align="right">     1 </TD> <TD> B </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 14 </TD> <TD align="right"> 1.0269 </TD> <TD align="right">    10 </TD> <TD> B </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 15 </TD> <TD align="right"> 1.0203 </TD> <TD align="right">    20 </TD> <TD> B </TD> <TD> 16:00:00 </TD> </TR>
  <TR> <TD align="right"> 16 </TD> <TD align="right"> 1.0217 </TD> <TD align="right">     1 </TD> <TD> B </TD> <TD> 20:00:00 </TD> </TR>
  <TR> <TD align="right"> 17 </TD> <TD align="right"> 1.0088 </TD> <TD align="right">    10 </TD> <TD> B </TD> <TD> 20:00:00 </TD> </TR>
  <TR> <TD align="right"> 18 </TD> <TD align="right"> 1.0017 </TD> <TD align="right">    20 </TD> <TD> B </TD> <TD> 20:00:00 </TD> </TR>
   </TABLE>


--- &vcenter .large

分析

### 感谢R 强大的分析功能

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Sun May 25 00:30:10 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> row.names  Estimate </TH> <TH> Std. Error </TH> <TH> t value </TH> <TH> Pr(&gt;|t|) </TH>  </TR>
  <TR> <TD align="right"> (Intercept) </TD> <TD align="right"> 1.0343 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> 1165.2725 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> Regularization10 </TD> <TD align="right"> -0.0139 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -15.6506 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> Regularization20 </TD> <TD align="right"> -0.0202 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -22.7578 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> FeatureSetB </TD> <TD align="right"> 0.0049 </TD> <TD align="right"> 0.0007 </TD> <TD align="right"> 6.7933 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> DayEffect20:00 </TD> <TD align="right"> -0.0162 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -18.2476 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> DayEffect9:00 </TD> <TD align="right"> 0.0035 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> 3.9816 </TD> <TD align="right"> 0.0018 </TD> </TR>
   </TABLE>


### 平均来说FeatureSet B 好 $0.5\%$

--- &vcenter .large

<img src="assets/img/cycle.png" class="fit100" />

--- &vcenter .large

成果

<div class="rimage center"><img src="assets/fig/unnamed-chunk-7.png" title="plot of chunk unnamed-chunk-7" alt="plot of chunk unnamed-chunk-7" class="plot" /></div>


--- .dark .segue

## 其他的工程面分享

### 处理大量数据需要很多工程

--- &vcenter .large

R Packages + Git + Jenkins:

开发后自动测试、部署和版本控制

<img src="assets/img/jenkins.png" class="fit50"/>

--- &vcenter .large

利用Spark做Data Cleaning

<img src="assets/img/Spark.png" class="fit50"/>

--- &vcenter .large

R + [rjson]() + [awscli](https://aws.amazon.com/cn/cli/)

自动利用AWS建立云端实验用Cluster


```r
ec2_request_spot_instances <- function(spot_price, instance_count, launch_specification = list()) {
  write(toJSON(launch_specification), file=(json.path <- tempfile()))
  cmd <- sprintf("aws ec2 request-spot-instances --spot-price %f --instance-count %d --launch-specification %s", spot_price, instance_count, sprintf("file://%s", json.path))
  system_collapse(cmd)
}
```


--- &vcenter .large

R 是可以对大量数据进行处理：

### 使用`Matrix`套件的稀疏矩阵
### `Rcpp`高效能的使用记忆体
### `Rcpp`整合第三方的库
### `pbdMPI`建立分散式平行运算丛集

--- &vcenter .large

关于今天分享的工作

感谢两位同学Y.-C. Juan, Y. Zhuang

和我在Bridgewell Inc.的合作

--- &vcenter .large

Q&A
