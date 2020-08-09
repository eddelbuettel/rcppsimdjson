RcppSimdJson Notes
================

  - [2020-08-08](#section)
      - [Enhancements](#enhancements)
          - [Better Queries](#better-queries)
          - [Compressed Files](#compressed-files)
          - [Smarter URL Handling](#smarter-url-handling)
      - [Fixes](#fixes)
          - [Lurking Windows String Encoding
            Trap](#lurking-windows-string-encoding-trap)

``` r
library(RcppSimdJson)
```

# 2020-08-08

… via experimental/pointer

## Enhancements

### Better Queries

We can still pass a single `query=` that’s applied to each `json=`
element.

``` r
json_to_query <- c(json1 = '["a",{"b":{"c": [[1,2,3],[4,5,6]]}}]',
                   json2 = '["a",{"b":{"c": [[7,8,9],[10,11,12]],"d":[[13,14,15,16],[17,18,19,20]]}}]')
#                                                                 ^^^ json1 doesn't have "d"
```

``` r
fparse(json_to_query, query = "1/b/c")
```

    ## $json1
    ##      [,1] [,2] [,3]
    ## [1,]    1    2    3
    ## [2,]    4    5    6
    ## 
    ## $json2
    ##      [,1] [,2] [,3]
    ## [1,]    7    8    9
    ## [2,]   10   11   12

But now we can also pass multiple “flat” queries (a named or unnamed
character vector). Each element of `query=` is applied to all elements
of `json=`.

This is the preferred method if each `json=` has roughly the same schema
and we want to extract the same data from each of them.

``` r
fparse(json_to_query, query = c(query1 = "1/b/c",
                                query2 = "1/b/c/0",
                                query3 = "1/b/c/1"))
```

    ## $json1
    ## $json1$query1
    ##      [,1] [,2] [,3]
    ## [1,]    1    2    3
    ## [2,]    4    5    6
    ## 
    ## $json1$query2
    ## [1] 1 2 3
    ## 
    ## $json1$query3
    ## [1] 4 5 6
    ## 
    ## 
    ## $json2
    ## $json2$query1
    ##      [,1] [,2] [,3]
    ## [1,]    7    8    9
    ## [2,]   10   11   12
    ## 
    ## $json2$query2
    ## [1] 7 8 9
    ## 
    ## $json2$query3
    ## [1] 10 11 12

When we want to extract different data from each `json=`, such as when
the schemata aren’t related, we can also specify a “nested” query. This
is a list of character vectors that are applied in a zip-like fashion.

``` r
fparse(json_to_query,
       query = list(queries1 = c(c1 = "1/b/c/0",
                                 c2 = "1/b/c/1"),
                    queries2 = c(d1 = "1/b/d/0",
                                 d2 = "1/b/d/1")))
```

    ## $queries1
    ## $queries1$c1
    ## [1] 1 2 3
    ## 
    ## $queries1$c2
    ## [1] 4 5 6
    ## 
    ## 
    ## $queries2
    ## $queries2$d1
    ## [1] 13 14 15 16
    ## 
    ## $queries2$d2
    ## [1] 17 18 19 20

### Compressed Files

We now handle .gz, .bz2, and .xz files that are decompressed to a raw
vector (via `memDecompress()`).

``` r
.read_compress_write_load <- function(file_path, temp_dir) {
    types <- c("gzip", "bzip2", "xz")
    exts <- c("gz",    "bz2",   "xz")

    init <- readBin(file_path, n = file.size(file_path), what = "raw")
    
    mapply(function(type, ext) {
        target_path <- paste0(temp_dir, "/", basename(file_path), ".", ext)
        writeBin(memCompress(init, type = type), target_path)
        RcppSimdJson::fload(target_path)
    }, types, exts, SIMPLIFY = FALSE)
}

my_temp_dir <- sprintf("%s/rcppsimdjson-compressed-files", tempdir())
dir.create(my_temp_dir)
all_files <- dir(
    system.file("jsonexamples", package = "RcppSimdJson"),
    recursive = TRUE,
    pattern = "\\.json$",
    full.names = TRUE
)
names(all_files) <- basename(all_files)
res <- t(sapply(all_files, .read_compress_write_load, my_temp_dir))
unlink(my_temp_dir)

stopifnot(all(apply(
    res, 1L, 
    function(.x) identical(.x[[1]], .x[[2]]) && 
        identical(.x[[1]], .x[[3]])
)))

res
```

    ##                                       gzip          bzip2         xz           
    ## apache_builds.json                    List,15       List,15       List,15      
    ## github_events.json                    List,8        List,8        List,8       
    ## instruments.json                      List,9        List,9        List,9       
    ## mesh.json                             List,8        List,8        List,8       
    ## numbers.json                          Numeric,10001 Numeric,10001 Numeric,10001
    ## random.json                           List,4        List,4        List,4       
    ## adversarial.json                      List,1        List,1        List,1       
    ## demo.json                             List,1        List,1        List,1       
    ## flatadversarial.json                  List,2        List,2        List,2       
    ## che-1.geo.json                        List,2        List,2        List,2       
    ## che-2.geo.json                        List,2        List,2        List,2       
    ## che-3.geo.json                        List,2        List,2        List,2       
    ## google_maps_api_compact_response.json List,4        List,4        List,4       
    ## google_maps_api_response.json         List,4        List,4        List,4       
    ## twitter_api_compact_response.json     List,16       List,16       List,16      
    ## twitter_api_response.json             List,25       List,25       List,25      
    ## repeat.json                           List,4        List,4        List,4       
    ## smalldemo.json                        List,8        List,8        List,8       
    ## truenull.json                         Logical,2000  Logical,2000  Logical,2000 
    ## twitter_timeline.json                 List,21       List,21       List,21      
    ## twitter.json                          List,2        List,2        List,2       
    ## twitterescaped.json                   List,2        List,2        List,2       
    ## update-center.json                    List,6        List,6        List,6

### Smarter URL Handling

With compressed files supported, we can better leverage the
`compressed_download=` parameter.

Additionally, remote JSON files are now downloaded simultaneously `if
(getOption("download.file.method", default = "auto") == "libcurl")`.

``` r
json_urls <- c(
    "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/smalldemo.json",
    "https://raw.githubusercontent.com/eddelbuettel/rcppsimdjson/master/inst/jsonexamples/small/demo.json"
)
```

``` r
my_temp_dir <- sprintf("%s/rcppsimdjson-downloads", tempdir())
dir.create(my_temp_dir)

fload(json_urls,
      query = list(c(width = "Thumbnail/Width", 
                     height = "Thumbnail/Height"),
                   c(width = "Image/Thumbnail/Width", 
                     height = "Image/Thumbnail/Height")),
      temp_dir = my_temp_dir,
      keep_temp_files = TRUE,
      compressed_download = TRUE)
```

    ## $smalldemo.json
    ## $smalldemo.json$width
    ## [1] 100
    ## 
    ## $smalldemo.json$height
    ## [1] 125
    ## 
    ## 
    ## $demo.json
    ## $demo.json$width
    ## [1] 100
    ## 
    ## $demo.json$height
    ## [1] 125

``` r
list.files(my_temp_dir)
```

    ## [1] "demo2ca65be3a426.json.gz"      "smalldemo2ca6718cd51a.json.gz"

## Fixes

### Lurking Windows String Encoding Trap

Windows was mangling non-ASCII UTF-8.

The issue/fix are essentially the same as
<https://github.com/SymbolixAU/jsonify/pull/57> and there’s now a test
using a mix of 1-4 byte characters.

``` r
extended_unicode <- '"լ ⿕  ٷ 豈 ٸ 㐀 ٹ 丂 Ɗ 一 á ٵ ̝ ѵ ̇ ˥ ɳ Ѡ · վ  й ף ޑ  ц Ґ  ӎ Љ ß ϧ ͎ ƽ ޜ է ϖ y Î վ Ο Ӊ ٻ ʡ ө ȭ ˅ ޠ ɧ ɻ ث ́ ܇ ܧ ɽ Ո 戸 Ð 坮 ٳ 䔢 찅 곂 묨 ß ᇂ ƻ 䏐 ܄ 㿕 ս ّ 昩 僫 똠 Ɯ ٰ É"'
fparse(extended_unicode)
```

    ## [1] "լ ⿕  ٷ 豈 ٸ 㐀 ٹ 丂 Ɗ 一 á ٵ ̝ ѵ ̇ ˥ ɳ Ѡ · վ  й ף ޑ  ц Ґ  ӎ Љ ß ϧ ͎ ƽ ޜ է ϖ y Î վ Ο Ӊ ٻ ʡ ө ȭ ˅ ޠ ɧ ɻ ث ́ ܇ ܧ ɽ Ո 戸 Ð 坮 ٳ 䔢 찅 곂 묨 ß ᇂ ƻ 䏐 ܄ 㿕 ս ّ 昩 僫 똠 Ɯ ٰ É"

``` r
fparse(charToRaw(extended_unicode))
```

    ## [1] "լ ⿕  ٷ 豈 ٸ 㐀 ٹ 丂 Ɗ 一 á ٵ ̝ ѵ ̇ ˥ ɳ Ѡ · վ  й ף ޑ  ц Ґ  ӎ Љ ß ϧ ͎ ƽ ޜ է ϖ y Î վ Ο Ӊ ٻ ʡ ө ȭ ˅ ޠ ɧ ɻ ث ́ ܇ ܧ ɽ Ո 戸 Ð 坮 ٳ 䔢 찅 곂 묨 ß ᇂ ƻ 䏐 ܄ 㿕 ս ّ 昩 僫 똠 Ɯ ٰ É"
