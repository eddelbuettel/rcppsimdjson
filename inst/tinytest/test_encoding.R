if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

# unicode_chr <- "լ ⿕  ٷ 豈 ٸ 㐀 ٹ 丂 Ɗ 一 á ٵ ̝ ѵ ̇ ˥ ɳ Ѡ · վ  й ף ޑ  ц Ґ  ӎ Љ ß ϧ ͎ ƽ ޜ է ϖ y Î վ Ο Ӊ ٻ ʡ ө ȭ ˅ ޠ ɧ ɻ ث ́ ܇ ܧ ɽ Ո 戸 Ð 坮 ٳ 䔢 찅 곂 묨 ß ᇂ ƻ 䏐 ܄ 㿕 ս ّ 昩 僫 똠 Ɯ ٰ É"
unicode_chr <- "\u056c \u2FD5  \u0677 \uf900 \u0678 \u3400 \u0679 \u4e02 \u018a \u4e00 \u00e1 \u0675 \u031d \u0475 \u0307 \u02e5 \u0273 \u0460 \u0387 \u057e  \u0439 \u05e3 \u0791  \u0446 \u0490  \u04ce \u0409 \u00df \u03e7 \u034e \u01bd \u079c \u0567 \u03d6 y \u00ce \u057e \u039f \u04c9 \u067b \u02a1 \u04e9 \u022d \u02c5 \u07a0 \u0267 \u027b \u062b \u0341 \u0707 \u0727 \u027d \u0548 \u6238 \u00d0 \u576e \u0673 \u4522 \ucc05 \uacc2 \ubb28 \u00df \u11c2 \u01bb \u43d0 \u0704 \u3fd5 \u057d \u0651 \u6629 \u50eb \ub620 \u019c \u0670 \u00c9"

# simple string
target <- unicode_chr
test <- sprintf('"%s"', unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
# object key and value
target <- `names<-`(list(unicode_chr), unicode_chr)
test <- sprintf('{"%s":"%s"}', unicode_chr, unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
# array
target <- c(unicode_chr, unicode_chr)
test <- sprintf('["%s","%s"]', unicode_chr, unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
# array (to matrix)
target <- matrix(unicode_chr, nrow = 2L, ncol = 2L)
test <- sprintf('[["%s","%s"],["%s","%s"]]',
                unicode_chr, unicode_chr, unicode_chr, unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
# object key and value
target <- `names<-`(list(unicode_chr), unicode_chr)
test <- sprintf('{"%s":"%s"}', unicode_chr, unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
# array of objects (to data frame)
target <- structure(list(c(unicode_chr, unicode_chr)),
                    class = "data.frame",
                    names = unicode_chr,
                    row.names = 1:2)
test <- sprintf('[{"%s":"%s"},{"%s":"%s"}]',
                unicode_chr, unicode_chr, unicode_chr, unicode_chr)
expect_identical(fparse(test), target)
expect_identical(fparse(charToRaw(test)), target)
