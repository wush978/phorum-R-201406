file.copy("index.Rmd", "index.Rmd.bak", overwrite=TRUE)
zh2TW <- readLines("zh2TW.txt")
zh2TW <- strsplit(zh2TW, " ", fixed=TRUE)

src <- readLines("index.Rmd")

for(i in seq_along(zh2TW)) {
  key <- zh2TW[[i]][2]
  value <- zh2TW[[i]][1]
  src <- gsub(key, value, src, fixed=TRUE)
}
write(src, file="index.Rmd2")
