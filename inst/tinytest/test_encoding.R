if (RcppSimdJson:::.unsupportedArchitecture()) exit_file("Unsupported chipset")

random_utf8 <- "լR<zٹ؄ƊGDáͽPٵ̝M5-uѵ̇˥)JYEɳĂѠ·վx?,:>й>SףޑR#цҐ5ӎ[0)ؾȸğͲ]ЉßQϧ͎ƽޜ@էϖyÎ&,uٜ w(ZP+dăվ˵  ΄ݤԨ؃0fΟӉٻkʡө_ȭv"

# simple string
expect_identical(fparse(sprintf('"%s"', random_utf8)), random_utf8)
# object key and value
expect_identical(fparse(sprintf('{"%s":"%s"}', random_utf8, random_utf8)),
                 `names<-`(list(random_utf8), random_utf8))
# array
expect_identical(fparse(sprintf('["%s","%s"]', random_utf8, random_utf8)),
                 c(random_utf8, random_utf8))
# array (to matrix)
expect_identical(fparse(sprintf('[["%s","%s"],["%s","%s"]]',
                                random_utf8, random_utf8, random_utf8, random_utf8)),
                 matrix(random_utf8, nrow = 2L, ncol = 2L))
# object key and value
expect_identical(fparse(sprintf('{"%s":"%s"}', random_utf8, random_utf8)),
                 `names<-`(list(random_utf8), random_utf8))
# array of objects (to data frame)
expect_identical(fparse(sprintf('[{"%s":"%s"},{"%s":"%s"}]',
                                random_utf8, random_utf8, random_utf8, random_utf8)),
                 structure(list(c(random_utf8, random_utf8)),
                           class = "data.frame",
                           names = random_utf8,
                           row.names = 1:2))
