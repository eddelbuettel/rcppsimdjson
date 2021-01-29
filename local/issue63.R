metric <- '{
            "metric":{
               "__name__":"node_cpu_seconds_total",
               "alias":"datacenteraggregation",
               "cluster":"someCluster",
               "cpu":"0",
               "datacenter":"some-Datacenter",
               "instance":"instance.endpoint:1234",
               "job":"clusters",
               "mode":"iowait"
            },
            "value":[
               12345656.643,
               "12345656.643"
            ]
         }'

test <- paste0('{
   "status":"success",
   "data":{
      "resultType":"vector",
      "result":[
         {
            "metric":{
               "__name__":"node_cpu_seconds_total",
               "alias":"someAlias",
               "cluster":"someCluster",
               "cpu":"0",
               "datacenter":"some-Datacenter",
               "instance":"instance.endpoint:1234",
               "job":"clusters",
               "mode":"idle"
            },
            "value":[
               12345656.643,
               "12345656.643"
            ]
         },
         ',paste(lapply(1:200000,function(x) paste(metric)),collapse=","),'
      ]
   }
}')

test <- RcppSimdJson::fparse(test)
cat("Still here.\n")
