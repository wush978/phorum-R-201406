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



<q><span class = 'white'>巨量資料<br/>掀起生活、工作和思考方式的革新</span></q>

--- &vcenter .large

巨量資料很熱

--- &vcenter .large

為了發揮資料的價值，我們要向資料學習

### 大資料不重要，大價值才重要

--- &vcenter .large

但是資料太多，學習時就遇到困難

--- &vcenter .large

挖資訊，變得很慢

### 畫張圖給老闆就花了我一整天了

### 請不要忘記「抽樣」這招，但是怎麼抽？

--- .dark .segue .quote

## Large Scale Learning in R

--- &vcenter .large

今天，希望能分享跨過門檻的經驗

和所有目前用R 遇到資料太大的同好分享

<img src="assets/img/cooperation.jpeg" class="fit100" </img>

--- &twocol .large

*** =left

## 關於講者

臺大電機所博士生

和宇匯知識科技合作建立廣告推薦引擎

### 今天的內容是我和Y.-C. Juan, Y. Zhuang的共同工作

*** =right

<img class="fit100" src="assets/img/wush.jpg"/>

--- &vcenter .large

不是教大家用R 處理GB/TB/PB等級的資料

### 多大的資料才算大資料？

--- &vcenter .large

只要當你「覺得」，R 處理資料太慢了

希望能在這次演講中學到解決的方法

--- &vcenter .large

找出慢的理由

### 記憶體不夠了
### 迴圈的效能太差了
### 資料讀取的太慢了
### 演算法的時間複雜度太差了

--- &vcenter .large

找出慢的理由

整體測試

`Rprof`

--- &vcenter .large

`Rprof`示範(參考[stackoverflow](http://stackoverflow.com/questions/3650862/how-to-efficiently-use-rprof-in-r))

--- &twocol

*** =left

### 一個大data.frame


```r
n <- 1000
x <- data.frame(group = 
    sample(letters[1:4], 
    n, replace=TRUE), 
  condition = 
    sample(LETTERS[1:10], n, 
           replace = TRUE), 
  data = rnorm(n))
```


### 合理的算法


```r
marginal.means.1 <- aggregate(
  data ~ group + condition, 
  data = x, FUN=mean)
```


*** =right

### 不合理的算法


```r
marginal.means.2 <- 
  marginal.means.1[NULL,]

row.counter <- 1
for (condition in levels(x$condition)) {
  for (group in levels(x$group)) {  
    # 省略掉...
  }
}
```


### 使用Rprof


```r
Rprof("profile1.out", line.profiling=TRUE)
eval(parse(file = 
  "http://pastebin.com/download.php?i=KjdkSVZq", 
  keep.source=TRUE))
Rprof(NULL)
```


---

### 解讀`profile1.out`


```r
summaryRprof("profile1.out", lines = "show")$by.self
```


<!-- html table generated in R 3.0.2 by xtable 1.7-1 package -->
<!-- Wed Apr 16 23:02:12 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> self.time </TH> <TH> self.pct </TH> <TH> total.time </TH> <TH> total.pct </TH>  </TR>
  <TR> <TD align="right"> download.php?i=KjdkSVZq#17 </TD> <TD align="right"> 4.48 </TD> <TD align="right"> 96.97 </TD> <TD align="right"> 4.48 </TD> <TD align="right"> 96.97 </TD> </TR>
  <TR> <TD align="right"> download.php?i=KjdkSVZq#16 </TD> <TD align="right"> 0.06 </TD> <TD align="right"> 1.30 </TD> <TD align="right"> 0.06 </TD> <TD align="right"> 1.30 </TD> </TR>
  <TR> <TD align="right"> download.php?i=KjdkSVZq#6 </TD> <TD align="right"> 0.04 </TD> <TD align="right"> 0.87 </TD> <TD align="right"> 0.04 </TD> <TD align="right"> 0.87 </TD> </TR>
  <TR> <TD align="right"> &lt no location&gt  </TD> <TD align="right"> 0.02 </TD> <TD align="right"> 0.43 </TD> <TD align="right"> 0.02 </TD> <TD align="right"> 0.43 </TD> </TR>
  <TR> <TD align="right"> download.php?i=KjdkSVZq#3 </TD> <TD align="right"> 0.02 </TD> <TD align="right"> 0.43 </TD> <TD align="right"> 0.02 </TD> <TD align="right"> 0.43 </TD> </TR>
   </TABLE>


--- &twocol

## 怎麼知道記憶體不夠了？

### 查詢物件的記憶體用量


```r
object.size(iris)
```

```
## 7088 bytes
```


### 利用作業系統的工具查R 的記憶體用量

- Windows : Ctrl + Alt + Delete
- Linux : htop

--- &vcenter

<img src="assets/img/Windows.png" class="fit100"/>

--- &vcenter .large

物件的複製

Copy On Write

![](assets/img/CopyOnWrite.png)

--- &vcenter .large

利用`tracemem`指令可以追蹤物件的複製

--- &twocol

## `data.table`、`tracemem`使用範例

*** =left


```r
library(data.table)
data(mtcars)
tracemem(mtcars)
```

```
## [1] "<0x106c4a408>"
```

```r
set(mtcars, j = "mpg", value = NULL)
```


*** =right


```r
tracemem(mtcars)
```

```
## [1] "<0x106c4a408>"
```

```r
mtcars[, "mpg"] <- NULL
```

```
## tracemem[0x106c4a408 -> 0x106c8c4b0]: eval eval withVisible withCallingHandlers doTryCatch tryCatchOne tryCatchList tryCatch try handle evaluate_call evaluate in_dir block_exec call_block process_group.block process_group withCallingHandlers process_file knit ifelse force in_dir parse_page slidify 
## tracemem[0x106c8c4b0 -> 0x106faaa00]: [<-.data.frame [<- eval eval withVisible withCallingHandlers doTryCatch tryCatchOne tryCatchList tryCatch try handle evaluate_call evaluate in_dir block_exec call_block process_group.block process_group withCallingHandlers process_file knit ifelse force in_dir parse_page slidify
```


--- &twocol

## Rcpp的Modify In Place範例

*** =left


```cpp
#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]] 
SEXP fill(NumericVector v, double value) {
  v.fill(value);
  return v;
} 
```


*** =right


```r
a <- numeric(100)
tracemem(a)
```

```
## [1] "<0x107401170>"
```

```r
invisible(fill(a, 1))
head(a)
```

```
## [1] 1 1 1 1 1 1
```

```r
a[] <- 2
```

```
## tracemem[0x107401170 -> 0x10733eaa0]: eval eval withVisible withCallingHandlers doTryCatch tryCatchOne tryCatchList tryCatch try handle evaluate_call evaluate in_dir block_exec call_block process_group.block process_group withCallingHandlers process_file knit ifelse force in_dir parse_page slidify
```

```r
head(a)
```

```
## [1] 2 2 2 2 2 2
```


--- &twocol

## 記憶體不夠怎麼辦？

- 買記憶體或買新電腦吧！（四萬塊可以買到64G）
- 使用資料庫系統，更有效率的使用硬碟
- 使用更有效率的資料結構：
    - 矩陣大部分是0 的時候，使用Matrix套件的Sparse Matrix結構
    - 大量重復的字串資料，可試著使用factor物件來節省記憶體

*** =left


```r
library(Matrix)
data(KNex)
class(KNex$mm)[1]
```

```
## [1] "dgCMatrix"
```

```r
object.size(KNex$mm)
```

```
## 109336 bytes
```


*** =right


```r
mm <- as(KNex$mm, "matrix")
class(mm)
```

```
## [1] "matrix"
```

```r
object.size(mm)
```

```
## 10537968 bytes
```


--- &vcenter .large

加快運算速度

--- &vcenter .large

改善的方法

### 利用compiler::cmpfun來將函數編譯成byte code
### 使用別人寫好的高效能套件
### 使用parallel套件來運用CPU的多核心
### 使用Rcpp套件來快速開發C++程式提升效能
### 使用更快的BLAS套件

--- 

## Compiler 效能比較


```r
library(compiler)
f <- function(n, x = 1) for (i in 1:n) x = 1/(1 + x)
lf <- cmpfun(f)
N <- 1e+06
library(rbenchmark)
benchmark(f(N, 1), lf(N, 1))
```

```
##       test replications elapsed relative user.self sys.self user.child
## 1  f(N, 1)          100   74.54    3.714     73.89    0.299          0
## 2 lf(N, 1)          100   20.07    1.000     19.67    0.315          0
##   sys.child
## 1         0
## 2         0
```


--- &vcenter .large

包成套件後，預設會使用byte code

### DESCRIPTION: ByteCompile

--- &vcenter .large

R's BLAS v.s. Apple's BLAS (可參考[RMacOSX-FAQ](http://r.research.att.com/man/RMacOSX-FAQ.html#Which-BLAS-is-used-and-how-can-it-be-changed_003f))

<!-- html table generated in R 3.0.2 by xtable 1.7-1 package -->
<!-- Fri Apr 18 21:20:09 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> Operation </TH> <TH> Dimension </TH> <TH> BLAS </TH> <TH> Apple.s.BLAS </TH>  </TR>
  <TR> <TD align="right"> 1 </TD> <TD> Creation, transp., deformation </TD> <TD> 2500x2500 </TD> <TD> 1.04 </TD> <TD> 1.05 </TD> </TR>
  <TR> <TD align="right"> 2 </TD> <TD> normal random matrix </TD> <TD> 2400x2400 </TD> <TD> 0.17 </TD> <TD> 0.2 </TD> </TR>
  <TR> <TD align="right"> 3 </TD> <TD> sorting </TD> <TD> 7M </TD> <TD> 0.67 </TD> <TD> 0.68 </TD> </TR>
  <TR> <TD align="right"> 4 </TD> <TD> Cross-product </TD> <TD> 2800x2800 </TD> <TD> 11.91 </TD> <TD> <font color='red'>0.55</font> </TD> </TR>
  <TR> <TD align="right"> 5 </TD> <TD> linear regr. </TD> <TD> 3000x3000 </TD> <TD> 5.48 </TD> <TD> <font color='red'>0.33</font> </TD> </TR>
  <TR> <TD align="right"> 6 </TD> <TD> FFT </TD> <TD> 2.4M </TD> <TD> 0.45 </TD> <TD> 0.45 </TD> </TR>
  <TR> <TD align="right"> 7 </TD> <TD> Eigenvalues </TD> <TD> 640x640 </TD> <TD> 0.82 </TD> <TD> 0.42 </TD> </TR>
  <TR> <TD align="right"> 8 </TD> <TD> Determinant </TD> <TD> 2500x2500 </TD> <TD> 3.84 </TD> <TD> <font color='red'>0.45</font> </TD> </TR>
  <TR> <TD align="right"> 9 </TD> <TD> Cholesky Decomp. </TD> <TD> 3000x3000 </TD> <TD> 4.56 </TD> <TD> <font color='red'>0.32</font> </TD> </TR>
  <TR> <TD align="right"> 10 </TD> <TD> Inverse </TD> <TD> 1600x1600 </TD> <TD> 3.77 </TD> <TD> <font color='red'>1.25</font> </TD> </TR>
   </TABLE>


--- &vcenter .large

Parallel & Bootstrap




```
boot(df, f, R = 100, parallel = "multicore", ncpus = 4)
2                   boot(df, f, R = 100, parallel = "no")
  replications elapsed relative user.self sys.self user.child
1           20 311.831    1.000     6.388    0.560    876.979
2           20 622.117    1.995   620.335    0.112      0.000
  sys.child
1      5.26
2      0.00
```

--- &vcenter .large

改善資料讀取的速度

--- &vcenter .large

加強資料讀取的效能

### 儲存成壓縮格式（把硬碟讀取時間變成CPU的時間）
### SSD
### 分散式讀取

--- &vcenter .large

Spark 

<img src="assets/img/Spark.png" class="fit100"/>

--- &vcenter .large

Single Machine

```
2014-04-17 19:20:02 INFO::Table 201404071810 is existed
2014-04-17 19:20:02 INFO::Table 201404071850 is existed
...
2014-04-17 19:21:37 INFO::parsing 047/20140417/ExposeLog201404171720.txt ...
2014-04-17 19:21:40 INFO::parsing 025/20140417/ExposeLog201404171720.txt ...
2
...
2014-04-17 19:27:22 INFO::parsing 017/PersonLog201404171730.txt
2014-04-17 19:27:46 INFO::parsing 017/PersonLog201404171730.txt
...
2014-04-17 19:46:14 INFO::{
    "ETag": "\"1467fb79cc5ea56dc156ab37eeb413cb\""
}
```

--- &twocol

*** =left

## 單台機器讀取

<img src="assets/img/1node.png" class="fit100" />

*** =right

## 多台機器讀取

<img src="assets/img/nnode.png" class="fit100" />

--- &vcenter .large

演算法

--- &vcenter .large

如何改善演算法及實作的效能

### 改善資料結構
### 改善最佳化的演算法

--- &vcenter .large

查詢的效能比較

### (取自[ptt](http://www.ptt.cc/bbs/R_Language/M.1396729914.A.B34.html))

從iris選取Sepal.Width為3.5的row

m: `data.frame`

m2: `data.table` with indexing

m3: `matrix`

--- &vcenter .large

200倍的iris(3萬rows)

```r
                                          expr replications elapsed relative
4                filter(m, Sepal.Width == 3.5)          100    0.05      1.0
5               filter(m2, Sepal.Width == 3.5)          100    0.14      2.8
7 m[fmatch(m$Sepal.Width, 3.5, nomatch = 0), ]          100    0.25      5.0
1                    m[m$Sepal.Width == 3.5, ]          100    0.44      8.8
8 m2[fmatch(m2$Sepal.Width, 3.5, nomatch = 0)]          100    0.33      6.6
3                                   m2[J(3.5)]          100    0.17      3.4
6                                m2[list(3.5)]          100    0.14      2.8
9                         m3[m3[, 2] == 3.5, ]          100    0.22      4.4
2                subset(m, Sepal.Width == 3.5)          100    0.55     11.0
```

--- &vcenter .large

5000倍的iris(75萬rows)

```r
                                          expr replications elapsed relative
4                filter(m, Sepal.Width == 3.5)          100    1.46    5.214
5               filter(m2, Sepal.Width == 3.5)          100    0.84    3.000
7 m[fmatch(m$Sepal.Width, 3.5, nomatch = 0), ]          100    6.46   23.071
1                    m[m$Sepal.Width == 3.5, ]          100   10.71   38.250
8 m2[fmatch(m2$Sepal.Width, 3.5, nomatch = 0)]          100    7.37   26.321
3                                   m2[J(3.5)]          100    0.28    1.000
6                                m2[list(3.5)]          100    0.34    1.214
9                         m3[m3[, 2] == 3.5, ]          100    4.96   17.714
2                subset(m, Sepal.Width == 3.5)          100   13.67   48.821
```

--- .segue .dark

## 實務經驗分享

--- &vcenter .large

廣告無所不在

<img src="assets/img/disp.png" class="fit100" />

--- &vcenter .large

<img src="assets/img/OnlineAd.png" class="fit100" />

--- &vcenter .large

從大量的瀏覽記錄中，學習出推薦方法

<img src="assets/img/Data.png" class="fit100" />

--- &vcenter .large

具體而言，

以下是「從資料學習」的一個起點

--- &vcenter .large

Logistic Regression

$$P( y | x ) = \frac{1}{1 + e^{- y w^T x}}$$

### $y$: 有沒有點擊
### $x$: 相關參數
### $w \in \mathbb{R}^d$: 模型

--- &vcenter .large

手上有這些資料

$$(y_1, x_1), (y_2, x_2), ...(y_n, x_n)$$

--- &vcenter .large

$n \sim 10^9$ 系統更新後指數會再往上跑

$d \sim 10^4$ 至 $10^7$

--- &vcenter .large

Regularized Loss:

$$f(w) = \frac{1}{2} w^T w + C \sum_{i=1}^n {log(1 + e^{-y_i w^T x_i})}$$

定出 $(y_1, x_1), ...$ 之後，
理想的模型 $w$ 是讓 $f$ 最小的點*

--- &vcenter .large

[Google](http://static.googleusercontent.com/media/research.google.com/zh-TW//pubs/archive/41159.pdf):

$f$要小

$w$要短(非0的要少)*

--- &vcenter .large

讓我們看看一個演算法的研究方向

--- &vcenter .large

從牛頓法開始

如果已經有模型 $w_k$，我們要找更好地點

找$s$ 使得以下式子最小：

$$\nabla f(w_k) ^T s + \frac{1}{2} s^T \nabla^2 f(w_k) s$$

--- &vcenter .large

對 $s$ 微一下

$s$要滿足

$$\nabla^2 f(w_k) s = - \nabla f(w_k)$$

--- &vcenter .large

Hessian矩陣 $\nabla^2 f(w) \in \mathbb{R}^{n \times n}$

但是可以寫成

$$I_n + CX^T D X$$

### $X$ 是資料矩陣
### $D$ 是對角化矩陣

--- &vcenter .large

可以用Conjugate Gradient的方法解

$$s + C X^T D X s = - \nabla f(w_k)$$

只要算 Hessian和vector的乘積 

我們不需要知道Hessian就可以找$w$

--- &vcenter .large

矩陣和向量的乘法可以平行化!

<img src="assets/img/SA2.png" class="fit100" />

--- &vcenter .large

理論搞懂了，要實做！

把[LIBLINEAR](http://www.csie.ntu.edu.tw/~cjlin/liblinear/)和[MPI](http://en.wikipedia.org/wiki/Message_Passing_Interface)參在一起

把[Matrix](http://cran.r-project.org/web/packages/Matrix/index.html)、[Rcpp](http://dirk.eddelbuettel.com/code/rcpp.html)和[pbdMPI](http://en.wikipedia.org/wiki/Programming_with_Big_Data_in_R)參在一起

可參考[pbdMPI](http://programmermagazine.github.io/201310/htm/article6.html)和[HsTrust](https://bitbucket.org/wush978/largescalelogisticregression/src/4daf9c5bba5cd0e4f35afd813866e6da72ca92bb/?at=hstrust)

--- &vcenter .large

[Matrix](http://cran.r-project.org/web/packages/Matrix/index.html)在重複計算Hessian太慢了

要改進

[Rcpp](http://dirk.eddelbuettel.com/code/rcpp.html)再度救援

--- &vcenter .large

辛苦寫出來，也不知道對不對

有R 在手邊，馬上比較小資料的結果

--- &vcenter .large

我還想要更快

嘗試比較其他BLAS

--- &vcenter .large

沒有比我們目前的快

```
nnz = 1.000000e+06, rows = 1.000000e+05, cols = 1.000000e+03, density = 1.000000e-02, times = 100, nr_threads = 1
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

<!-- html table generated in R 3.0.2 by xtable 1.7-1 package -->
<!-- Thu Apr 17 22:07:53 2014 -->
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

<!-- html table generated in R 3.0.2 by xtable 1.7-1 package -->
<!-- Thu Apr 17 22:09:52 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> row.names  Estimate </TH> <TH> Std. Error </TH> <TH> t value </TH> <TH> Pr(&gt |t|) </TH>  </TR>
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

![plot of chunk unnamed-chunk-9](assets/fig/unnamed-chunk-9.png) 


--- .dark .segue

## 革命尚未成功，同志仍需努力

--- &vcenter .large

Online Logistic Regression

### 更快
### 更有彈性
### 更多因子

--- &vcenter .large

TRON v.s. FTRL

![plot of chunk unnamed-chunk-10](assets/fig/unnamed-chunk-10.png) 


--- .dark .segue

## 總結

--- &vcenter .large

從大量的數據學出價值，我們需要

### 提出問題
### 提出模型描述問題
### 運用工程能力實做方法
### 分析資料
### 評估與比較

--- &vcenter .large

Q&A
