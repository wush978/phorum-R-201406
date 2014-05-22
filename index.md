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
--- .quote .segue .nobackground .dark



--- .dark .segue .quote

## Large Scale Learning in R

--- &vcenter .large

很多人都在詬病R 無法處理大量的數據

--- &vcenter .large

但是只要用對R 的包

R 是可以處理大量的數據

--- &vcenter .large

今天我想跟大家分享

運用R 建立商用推薦引擎的心得

主要是靠著`Rcpp`和`pbdMPI`等包

打造可扩放性的學習模組

--- &vcenter .large

實際的數據和課本上的數據是不一樣的

--- &twocolvcenter

*** =left

## 實際的數據

- 亂、充滿錯誤
- 不停的變動
- 不知道怎樣才算好
    - 只能不斷精益求精

*** =right

## 課本的數據

- 乾淨
- 靜止
- 需要複雜的演算法

--- &vcenter .large

今天從這樣的數據開始

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Thu May 22 21:01:35 2014 -->
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
以「預測點擊發生的機率」為例

--- &twocol .large

## 問題的建模

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Thu May 22 21:01:35 2014 -->
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

實際的資料

有 $10^{9+}$ 筆資料

有 $10^{3+}$ 筆廣告

每種類別變數可能到 $10^{3}$ 類別

--- &vcenter .large

`model.matrix`的後果：

至少需要 $10^9 \times (10^3...)$ 個位元

也就是 $116.415$ GB的記憶體

--- &vcenter .large

開始抱怨了!

### R 不能處理大量的數據

### R 會吃很多記憶體

### 使用R 跑不動，使用其他工具就跑得動

--- &vcenter .large

請使用和其他工具一樣的資料結構:

這種狀況，應該使用稀疏矩阵

--- &twocolvcenter

*** =left


```r
library(Matrix); data(KNex)
class(KNex$mm)[1]
```

```
[1] "dgCMatrix"
```

```r
object.size(KNex$mm)
```

```
109336 bytes
```


*** =right


```r
mm <- as(KNex$mm, "matrix")
class(mm)
```

```
[1] "matrix"
```

```r
object.size(mm)
```

```
10537968 bytes
```


--- &vcenter .large

在我手上的資料上

大概可以省下 $10^2 \sim 10^3$ 倍記憶體

`並且可以大幅加快運算效能`

--- &vcenter .large

還可用`Rcpp`實做矩陣-向量的乘法

我們的實作和BLAS庫比較：

```
nnz = 1.000000e+06, rows = 1.000000e+05, cols = 1.000000e+03, 
density = 1.000000e-02, times = 100, nr_threads = 1
generating data...done 453ms
CSR Xv:  388ms
CSR XTv: 288ms
CSR X^TDXv: 488ms
CSR X^TDXv (ACC): 367ms
CSR Xv (MKL):  468ms
CSR XTv (MKL): 482ms
CSR X^TDXv (MKL): 529ms
CSR Xv (NIST):  398ms
CSR XTv (NIST): 461ms
error: 0.000000
```

--- &vcenter .large

再透過`pbdMPI`開發分散式矩陣乘法

利用更多CPU和更多記憶體提升效能

$$\left(\begin{array}{c}X_1 \\ X_2\end{array}\right) w = \left(\begin{array}{c}X_1 w \\ X_2 w\end{array}\right)$$

--- &vcenter .large

`pbdMPI`是[pbdR(Programming with Big Data in R)](http://r-pbd.org/)專案的套件之一

--- &twocolvcenter .large

*** =left

## pbdMPI的優點

### 好安裝
### 好開發

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

## 最佳化演算法

### 快速
### 不用計算Hessian矩陣
### 很棒的實作[LIBLINEAR](http://www.csie.ntu.edu.tw/~cjlin/liblinear/)

*** =right

<h3><a href="http://dx.doi.org/10.1145/1273496.1273567">Lin et al. 2007 Trust Region Newton Methods for Large-scale Logistic Regression</a></h3>

--- &vcenter .large

為了加強效能

並且能夠更方便的更改模型

我們自己包LIBLINEAR到R中

--- &vcenter .large

## LIBLINEAR 原始碼中的 `tron.h`


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


- TRON是最佳化的核心演算法的實作
- TRON沒有牽涉到資料，甚至沒有牽涉到Loss Function
- `function`提供了一個界面

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
- `Hv`是`fun`的hessian乘上一個向量

--- &twocolvcenter .large

## 利用`Rcpp Modules`把`function`暴露到R中

*** =left

### 實作Rfunction


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

將Rfunction暴露出來


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

實作的結果可包裝成[套件](https://bitbucket.org/wush978/largescalelogisticregression/src/4daf9c5bba5cd0e4f35afd813866e6da72ca92bb/?at=hstrust)



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

結合`pbdMPI`和`Rcpp Modules`

讓TRON呼叫分散式的運算系統

SPMD架構，無master，一次資料交換


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

`objective_function`非常易於修改

所以我們能將注意力專注於模型上


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

終於跨過資料量的門檻了...

該看看資料了！

--- .dark .segue

## 資料越大，結果就會越好嗎？

---

<img src="assets/img/datasize.png" class="fit100"/>

--- .dark .segue

## 不同的模型對預測會有影響嗎？

--- &vcenter .large

因子的組合

<img src="assets/img/factors.png" class="fit100" />

--- &vcenter .large

實驗結果

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Thu May 22 21:01:37 2014 -->
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

### 感謝R 強大的分析功能

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Thu May 22 21:01:37 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> row.names  Estimate </TH> <TH> Std. Error </TH> <TH> t value </TH> <TH> Pr(&gt;|t|) </TH>  </TR>
  <TR> <TD align="right"> (Intercept) </TD> <TD align="right"> 1.0343 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> 1165.2725 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> Regularization10 </TD> <TD align="right"> -0.0139 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -15.6506 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> Regularization20 </TD> <TD align="right"> -0.0202 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -22.7578 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> FeatureSetB </TD> <TD align="right"> 0.0049 </TD> <TD align="right"> 0.0007 </TD> <TD align="right"> 6.7933 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> DayEffect20:00 </TD> <TD align="right"> -0.0162 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> -18.2476 </TD> <TD align="right"> 0.0000 </TD> </TR>
  <TR> <TD align="right"> DayEffect9:00 </TD> <TD align="right"> 0.0035 </TD> <TD align="right"> 0.0009 </TD> <TD align="right"> 3.9816 </TD> <TD align="right"> 0.0018 </TD> </TR>
   </TABLE>


### 平均來說FeatureSet B 好 $0.5\%$

--- &vcenter .large

<img src="assets/img/cycle.png" class="fit100" />

--- &vcenter .large

成果

<div class="rimage center"><img src="assets/fig/unnamed-chunk-7.png" title="plot of chunk unnamed-chunk-7" alt="plot of chunk unnamed-chunk-7" class="plot" /></div>

