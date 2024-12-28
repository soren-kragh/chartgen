std::cout << R"EOF(
Title: Global Energy Consumption
SubSubTitle: (source www.ourworldindata.org)

Axis.X.Label: Year
Axis.X.Grid: On Off
Axis.X.GridStyle: Solid

Axis.PriY.Style: Edge
Axis.PriY.NumberUnit: _TWh
Axis.PriY.Tick: 20000 2
Axis.PriY.GridStyle: Solid

Axis.SecY.Style: Edge
Axis.SecY.Unit:
  billion
  people
Axis.SecY.Label: Population

Series.Type: StackedArea
Series.New: Other renewables
Series.New: Biofuels
Series.New: Solar
Series.New: Wind
Series.New: Hydropower
Series.New: Nuclear
Series.New: Gas
Series.New: Oil
Series.New: Coal
Series.New: Traditional biomass

Series.Type: Line
Series.New: Global population
Series.AxisY: Secondary
Series.LineWidth: 5
Series.LineDash: 0
Series.LineColor: black

# The X-axis is textual for StackedArea, so it is our responsibility to space
# out gaps between years in case equidistant years is desired. Use tilde (~) for
# years with no data in order to get a connected line.
Series.Data:
  1800  0          0          0           0           0         0         0         0         97        5556    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1810  0          0          0           0           0         0         0         0         128       5833    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1820  0          0          0           0           0         0         0         0         153       6111    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1830  0          0          0           0           0         0         0         0         264       6389    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1840  0          0          0           0           0         0         0         0         356       6944    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1850  0          0          0           0           0         0         0         0         569       7222    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1860  0          0          0           0           0         0         0         0         1061      6944    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1870  0          0          0           0           0         0         0         6         1642      6944    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1880  0          0          0           0           0         0         0         33        2542      6944    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1890  0          0          0           0           38.88889  0         33        89        3856      6667    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1900  0          0          0           0           47.22222  0         64        181       5728      6111    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1910  0          0          0           0           91.666664 0         142       397       8656      6389    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1920  0          0          0           0           177.77777 0         233       889       9833      6944    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1930  0          0          0           0           363.8889  0         603       1756      10125     7222    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1940  0          0          0           0           533.3333  0         875       2653      11586     7222    -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       -
  1950  0          0          0           0           925       0         2092      5444      12603     7500    2.493092843
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.536927039
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.584086332
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.634106246
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.685894869
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.740213784
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.795409978
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.852618346
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.911249658
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       2.965950358
  1960  0          0          0           0           1913.8889 0         4472      11097     15442     8889    3.015470890
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       3.064869656
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       3.123374299
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       3.192807820
     -  ~          ~          ~           ~           ~         ~         ~         ~         ~         ~       3.264487340
  1965  55.949085  9.029861   0           0           2729.8865 72.469345 6303.8203 18012.52  16177.694 9162    3.334533717
  1966  61.737606  9.209844   0           0           2909.136  97.70497  6868.814  19427.398 16359.431 9218    3.404041107
  1967  62.35338   9.383716   0           0           2973.9692 116.35517 7374.058  20779.828 16095.517 9274    3.473412894
  1968  69.03191   9.5538     0           0           3132.3052 147.87189 8044.3477 22511.285 16333.143 9331    3.545187239
  1969  72.558716  9.723981   0           0           3316.9805 175.31134 8833.434  24446.174 16847     9387    3.619491592
  1970  80.33795   13.501494  0           0           3473.4265 223.80801 9614.809  26672.713 17087.004 9444    3.694683801
  1971  87.339066  13.562146  0           0           3628.942  311.28723 10293.19  28117.908 16986.387 9499    3.769847834
  1972  93.40202   14.82311   0           0           3798.634  431.79926 10861.713 30331.375 17168.227 9553    3.844917687
  1973  100.24631  14.250557  0           0           3852.995  578.5737  11377.832 32722.135 17683.568 9608    3.920805030
  1974  106.87296  14.334716  0           0           4232.1367 756.48175 11659.859 32224.648 17696.432 9663    3.996416108
  1975  108.1472   13.688392  0           0           4284.3325 1049.4497 11659.736 31992.346 18045.436 9718    4.070735264
  1976  120.57106  13.751189  0           0           4267.3394 1227.7115 12354.139 34059.953 18699.785 9774    4.144246391
  1977  127.266624 18.318535  0           0           4410.859  1528.1375 12759.838 35189.734 19267.412 9830    4.217863819
  1978  135.02682  23.178774  0           0.008870971 4773.711  1775.7009 13293.947 36703.98  19467.49  9886    4.292097508
  1979  144.51552  28.023214  0           0.017741943 5012.647  1846.8246 14118.029 37182.266 20370.578 9943    4.368539472
  1980  153.78857  32.78612   0           0.031048333 5120.4688 2020.0956 14236.961 35561.332 20877.783 10000   4.447606208
  1981  164.9076   29.747923  0           0.031048333 5230.6284 2385.6428 14395.873 34295.703 21158.002 10106   4.528777285
  1982  191.2329   37.556244  0           0.054704163 5325.2656 2588.3174 14469.731 33270.605 21395.426 10213   4.612673436
  1983  211.77058  49.01924   0.008870971 0.09697445  5552.5425 2933.459  14703.826 33099.758 22050.97  10321   4.697327604
  1984  236.32065  57.806423  0.018661888 0.13234167  5740.62   3559.8567 15902.676 33624.367 23008.44  10430   4.782175500
  1985  243.1773   68.41009   0.03473722  0.18989833  5852.602  4224.8374 16262.217 33686.312 23996.738 10541   4.868943460
  1986  263.69818  84.20299   0.04489833  0.4105228   5931.936  4525.0864 16421.107 34717.92  24263.818 10653   4.958072822
  1987  282.46243  85.33012   0.031353053 0.57772696  6012.128  4922.331  17281.893 35513.047 25228.486 10765   5.049746378
  1988  290.88327  89.560196  0.030149443 0.980478    6204.7944 5366.448  18088.842 36628.914 25983.078 10879   5.141992550
  1989  322.37277  95.68924   0.77539134  7.835361    6172.972  5518.996  18888.648 37209.367 26231.049 10995   5.234431714
  1990  361.2137   106.56911  1.1483465   10.741176   6383.0127 5676.7207 19481.15  37608     25916.205 11111   5.327803075
  1991  376.84134  110.64682  1.4942344   12.082573   6530.634  5948.311  19972.836 37688.625 25667.406 11243   5.418735879
  1992  404.81308  108.705635 1.379965    13.994874   6529.961  5993.4634 20063.482 38192.87  25567.281 11376   5.505989821
  1993  418.26727  114.34005  1.6460894   16.867165   6923.2563 6199.8696 20265.48  38024.57  25689.945 11511   5.591544841
  1994  434.6814   124.101776 1.7652723   21.088448   6966.385  6316.248  20389.613 38852.32  25791.27  11647   5.675551281
  1995  454.28992  131.0704   1.8891695   24.46058    7343.3516 6590.213  21104.492 39431.895 25975.748 11785   5.758878997
  1996  466.36444  125.784515 2.0676155   27.250431   7442.1943 6828.8057 22159.254 40326.613 26592.398 11925   5.842055726
  1997  499.9769   130.03065  2.2167242   35.567307   7573.902  6781.843  22030.39  41422.08  26547.277 12066   5.924787787
  1998  520.5851   133.38396  2.4006376   47.10265    7631.793  6898.552  22433.998 41583.832 26380.13  12209   6.007066714
  1999  546.1845   135.24736  2.6769266   62.767708   7689.5576 7161.0522 23071.902 42266.777 26492.459 12414   6.089006330
  2000  571.59485  132.50879  3.1288824   92.87769    7826.087  7322.6826 23994.256 42983.43  27441.488 12500   6.171703018
  2001  593.6476   131.42497  4.1780615   112.75232   7575.298  7480.557  24316.832 43366.16  27864.717 12500   6.254936479
  2002  639.16516  149.31444  5.2459383   152.89377   7665.178  7551.0767 25028.285 43650.734 28967.592 12470   6.337730319
  2003  674.7164   168.93098  6.5339413   183.53323   7606.731  7350.6562 25727.73  44580.05  31511.33  12329   6.420361649
  2004  729.3295   201.0801   8.554863    246.72241   8116.2583 7635.7695 26734.273 46367.14  33689.836 12160   6.503377778
  2005  790.01874  234.50827  11.978459   299.59113   8336.13   7607.3535 27438.982 46965.55  36190.766 12076   6.586970145
  2006  841.3214   294.60794  16.379019   379.86957   8599.477  7653.7217 28161.389 47468.766 38073.164 11993   6.671452018
  2007  913.18494  391.5707   22.07666    484.9491    8687.611  7450.836  29315.703 48022.76  40233.902 11911   6.757308750
  2008  976.65955  535.71674  35.639404   622.1818    9137.198  7381.6016 30026.688 47628.992 40786.047 11829   6.844457675
  2009  1049.248   611.32715  58.8195     773.0152    9066.074  7232.23   29405.7   46566.066 40189.99  11747   6.932766459
  2010  1175.8253  692.14124  94.092354   961.4859    9520.543  7373.091  31593.82  48058.23  41988.312 11667   7.021732131
  2011  1235.6885  728.7748   181.25386   1215.6805   9634.818  7021.4863 32349.117 48400.316 43940.797 11553   7.110923797
  2012  1340.7991  757.5906   278.34863   1454.6155   9984.604  6500.3687 33203.137 49164.34  44060.996 11441   7.201202531
  2013  1447.8152  820.7763   377.7912    1732.0547   10323.586 6512.8066 33720.777 49654.363 44709.242 11330   7.291793608
  2014  1569.6025  866.74524  534.4909    1912.3323   10535.131 6606.008  33961.7   50011.83  44893.17  11220   7.381616239
  2015  1696.5197  900.52325  689.4229    2238.7305   10445.364 6655.106  34752.11  50977.21  43680.504 11111   7.470491904
  2016  1720.5377  941.4909   879.1254    2575.6736   10749.688 6714.155  35229.348 52060.85  42736.863 11111   7.558554580
  2017  1817.4558  960.00476  1185.098    3038.9753   10826.592 6734.279  36517.57  52978.8   43193.28  11111   7.645618013
  2018  1964.74    1039.946   1520.514    3361.3286   11080.882 6855.408  38321.438 53521.387 43852.793 11111   7.729902724
  2019  2063.5725  1119.6278  1859.5966   3746.7087   11184.757 7071.7817 39084.453 53618.926 43597.164 11111   7.811293714
  2020  2181.6753  1070.2726  2245.2683   4188.111    11449.073 6776.8657 38714.09  48745.684 42296.684 11111   7.887001284
     -  2351.194   1131.0247  2751.7534   4865.935    11222.464 7037.07   40239.016 51530.492 44600.08  11111   7.954448405
     -  2414.3037  1193.2262  3446.4075   5495.7437   11271.806 6703.874  40086.875 53226.84  44869.12  11111   8.021407170
  2023  2427.8613  1317.6246  4264.261    6040.359    11014.117 6824.1772 40101.74  54564     45564.93  11111   8.091734935

Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right
)EOF";