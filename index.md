---
title       : Large Scale Learning in R
subtitle    : 
author      : Wush Wu
job         : Taiwan R User Group
framework   : io2012        # {io2012, html5slides, shower, dzslides, ...}
highlighter : highlight.js  # {highlight.js, prettify, highlight}
hitheme     : zenburn       # 
widgets     : []            # {mathjax, quiz, bootstrap}
mode        : selfcontained # {standalone, draft}
license: by-nc-sa
logo: Taiwan-R-logo.png
--- &twocol




*** =left

## 關於講者

- 臺大電機所博士生
- 和宇匯知識科技合作
    - 建立廣告推薦引擎

*** =right

<img class="fit100" src="assets/img/wush.jpg"/>

--- .quote .segue .nobackground .dark

<q><span class = 'white'>巨量資料<br/>掀起生活、工作和思考方式的革新</span></q>

--- &vcenter .large

巨量資料很熱

--- &vcenter .large

但是資料多，真的有用嗎？

### Garbage in, garbage out

--- &vcenter .large

讓資料說話

### 讓資料自己來證明它是不是垃圾

---

## 如何驗證資料是不是垃圾？

- 整理資料
- **從資料中學習**
- 驗證學習的成果

--- .dark .segue .quote

## Large Scale Learning in R

--- &vcenter .large

這年頭，不「大」就不行了？

### 資料大不是重點，資料的價值才是重點

### 最近手上的資料也越來越大了

--- &vcenter .large

資料太大，建立了分析資料的門檻

### 畫張圖給老闆就花了我一整天了

### 請不要忘記「抽樣」這招

--- &vcenter .large

今天，我希望能分享跨過門檻的經驗

和所有目前用R 遇到資料太大的同好分享

<img src="assets/img/cooperation.jpeg" style="fit100" </img>

--- &vcenter .large

不是教大家用R 處理GB/TB/PB等級的資料

### 多大的資料才算大資料？

--- &vcenter .large

只要當你「覺得」，R 處理資料太慢了

---

## 找出慢的理由

- 記憶體不夠了
- 迴圈的效能太差了
- 資料讀取的太慢了
- 演算法的時間複雜度太差了

--- &vcenter .large

記憶體的效能，是硬碟的1000倍

---

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

--- 

## 物件的複製，Copy On Write

- 利用`tracemem`指令可以追蹤物件的複製
- 儘量不要修改物件的內容
- 修改大data.frame的時候，可以使用data.table套件
    - data.table::set 可以新增或刪除data.table中的column而不複製整個物件(請參考[Stack Overflow上的說明](http://stackoverflow.com/questions/17891519/is-it-possible-to-modify-a-data-frame-in-place-destructively?answertab=active#tab-top))
- 也可以使用Rcpp來直接修改物件的內容

---

## 記憶體不夠怎麼辦？

- 買記憶體或買新電腦吧！（四萬塊可以買到64G）
- 使用資料庫系統，更有效率的使用硬碟
- 使用更有效率的資料結構：
    - 矩陣大部分是0 的時候，使用Matrix套件的Sparse Matrix結構
    - 大量重復的字串資料，可以使用factor物件來節省記憶體

--- &vcenter .large

改善效能

---

## 評估效能

- 可利用`rbenchmark`或`microbenchmark`來測量效能
- 利用`Rprof`來測量整段程式中，各種函數的計算時間

---

## 加速R 的方法

- 利用compiler::cmpfun來將函數編譯成byte code
- 使用parallel套件來運用CPU的多核心
- 使用Rcpp套件來快速開發C++程式提升效能
- [利用OpenBLAS來加速矩陣運算](http://www.r-bloggers.com/for-faster-r-use-openblas-instead-better-than-atlas-trivial-to-switch-to-on-ubuntu/)
- 使用別人寫好的高效能套件

--- &vcenter .large

改善資料讀取的速度

---

## 克服資料讀取效能的方法

- 儲存成壓縮格式（把硬碟讀取時間變成CPU的時間）
- SSD
- 分散式叢集Hadoop, Spark

--- &vcenter .large

演算法

---

## 如何改善演算法的效能

- 了解自己需要的操作是什麼
- 查詢的操作，使用data.table套件的indexing，或是hash套件的hash
- 重複使用計算後的結果
- 改善最佳化的演算法
    - SGD v.s. Iterative Optimization
    - Online Learning

--- .segue .dark

## 經驗分享

---

## 自身專案經驗分享

### 目標

從大量的瀏覽記錄中，學習出推薦方法

---

## 單面像的學習
