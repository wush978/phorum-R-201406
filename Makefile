all : index.html

%.html : %.Rmd
	Rscript -e "library(slidify);slidify('$<')"
