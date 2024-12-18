std::cout << R"EOF(
Title: Climate Data
SubSubTitle: (source www.woodfortrees.org)

Axis.PriY.Label: Temperature Anomaly (C)
Axis.SecY.Label: Parts per Million Co2

# Despite the line being specified first, it'll be shown above the confidence
# interval below because Area plots are always drawn on a lower display layer.
Series.Type: Line
Series.New:
  BEST global
  land/ocean mean
Series.LineColor: crimson
Series.LineWidth: 1

Series.Type: Area
Series.New:
  BEST 95%
  confidence interval
Series.Base: -1.5
# Clear all persistent Style modifiers (the LineWidth given above), then
# specify light color for the confidence interval.
Series.Style: 0
Series.FillColor: crimson 0.8

Series.Type: Area
Series.New:
Series.Base: -1.5
# Use background color to obscure the fill below the low confidence value.
Series.FillColor: white

Series.Type: Line
Series.New:
  Mauna Loa
  atmospheric Co2
Series.LineColor: deepskyblue
Series.LineWidth: 3
Series.AxisY: Secondary

# A dash indicates no value.
# Year  Temp   Hi-cnf Lo-cnf Co2
Series.Data:
  1850 -0.769 -0.402 -1.136  -
    "" -0.177  0.170 -0.524  -
    "" -0.251  0.292 -0.794  -
    "" -0.360  0.060 -0.780  -
    "" -0.568 -0.165 -0.971  -
  1855 -0.051  0.469 -0.571  -
    ""  0.000  0.372 -0.372  -
    "" -0.546 -0.185 -0.907  -
    "" -0.236  0.167 -0.639  -
    "" -0.377 -0.066 -0.688  -
  1860 -0.350 -0.004 -0.696  -
    "" -1.020 -0.633 -1.407  -
    "" -0.945 -0.537 -1.353  -
    "" -0.155  0.190 -0.500  -
    "" -0.582 -0.205 -0.959  -
  1865  0.092  0.432 -0.248  -
    ""  0.055  0.425 -0.315  -
    "" -0.139  0.243 -0.521  -
    "" -0.486 -0.182 -0.790  -
    "" -0.192  0.153 -0.537  -
  1870 -0.287 -0.029 -0.545  -
    "" -0.550 -0.275 -0.825  -
    "" -0.457 -0.044 -0.870  -
    "" -0.316 -0.055 -0.577  -
    "" -0.224  0.005 -0.453  -
  1875 -0.468 -0.261 -0.675  -
    "" -0.386 -0.142 -0.630  -
    "" -0.191  0.064 -0.446  -
    "" -0.008  0.285 -0.301  -
    "" -0.129  0.082 -0.340  -
  1880 -0.370 -0.138 -0.602  -
    "" -0.115  0.111 -0.341  -
    ""  0.104  0.285 -0.077  -
    "" -0.409 -0.223 -0.595  -
    "" -0.237 -0.054 -0.420  -
  1885 -0.679 -0.474 -0.884  -
    "" -0.642 -0.478 -0.806  -
    "" -0.952 -0.739 -1.165  -
    "" -0.360 -0.194 -0.526  -
    "" -0.117  0.033 -0.267  -
  1890 -0.449 -0.289 -0.609  -
    "" -0.487 -0.326 -0.648  -
    "" -0.279 -0.136 -0.422  -
    "" -0.900 -0.742 -1.058  -
    "" -0.542 -0.401 -0.683  -
  1895 -0.453 -0.320 -0.586  -
    "" -0.213 -0.077 -0.349  -
    "" -0.165 -0.034 -0.296  -
    "" -0.161 -0.017 -0.305  -
    "" -0.207 -0.083 -0.331  -
  1900 -0.408 -0.280 -0.536  -
    "" -0.277 -0.137 -0.417  -
    "" -0.279 -0.151 -0.407  -
    "" -0.257 -0.145 -0.369  -
    "" -0.657 -0.531 -0.783  -
  1905 -0.346 -0.211 -0.481  -
    "" -0.339 -0.204 -0.474  -
    "" -0.408 -0.289 -0.527  -
    "" -0.414 -0.300 -0.528  -
    "" -0.766 -0.663 -0.869  -
  1910 -0.382 -0.282 -0.482  -
    "" -0.675 -0.573 -0.777  -
    "" -0.226 -0.129 -0.323  -
    "" -0.530 -0.432 -0.628  -
    ""  0.001  0.104 -0.102  -
  1915 -0.233 -0.115 -0.351  -
    "" -0.182 -0.068 -0.296  -
    "" -0.675 -0.563 -0.787  -
    "" -0.523 -0.404 -0.642  -
    "" -0.195 -0.068 -0.322  -
  1920 -0.169 -0.056 -0.282  -
    "" -0.114  0.000 -0.228  -
    "" -0.276 -0.167 -0.385  -
    "" -0.287 -0.182 -0.392  -
    "" -0.253 -0.145 -0.361  -
  1925 -0.414 -0.312 -0.516  -
    ""  0.233  0.339  0.127  -
    "" -0.244 -0.137 -0.351  -
    ""  0.109  0.215  0.003  -
    "" -0.413 -0.312 -0.514  -
  1930 -0.218 -0.112 -0.324  -
    "" -0.098  0.005 -0.201  -
    ""  0.109  0.210  0.008  -
    "" -0.246 -0.143 -0.349  -
    "" -0.256 -0.158 -0.354  -
  1935 -0.365 -0.257 -0.473  -
    "" -0.307 -0.215 -0.399  -
    "" -0.057  0.042 -0.156  -
    ""  0.080  0.174 -0.014  -
    "" -0.034  0.061 -0.129  -
  1940  0.066  0.169 -0.037  -
    "" -0.011  0.187 -0.209  -
    ""  0.342  0.543  0.141  -
    "" -0.048  0.155 -0.251  -
    ""  0.383  0.582  0.184  -
  1945  0.106  0.309 -0.097  -
    ""  0.051  0.203 -0.101  -
    "" -0.001  0.137 -0.139  -
    ""  0.074  0.205 -0.057  -
    ""  0.082  0.197 -0.033  -
  1950 -0.233 -0.133 -0.333  -
    "" -0.332 -0.247 -0.417  -
    ""  0.184  0.266  0.102  -
    ""  0.088  0.177 -0.001  -
    "" -0.212 -0.122 -0.302  -
  1955  0.209  0.293  0.125  -
    "" -0.074 -0.001 -0.147  -
    "" -0.122 -0.053 -0.191  -
    ""  0.353  0.423  0.283  -
    ""  0.106  0.172  0.040  315.52
  1960 -0.011  0.068 -0.090  316.38
    ""  0.053  0.106  0.000  316.84
    ""  0.058  0.106  0.010  317.89
    "" -0.003  0.046 -0.052  318.69
    "" -0.087 -0.038 -0.136  319.52
  1965 -0.040  0.010 -0.090  319.38
    "" -0.159 -0.107 -0.211  320.57
    "" -0.095 -0.049 -0.141  322.28
    "" -0.205 -0.166 -0.244  322.51
    "" -0.043  0.002 -0.088  323.95
  1970  0.137  0.171  0.103  325.01
    "" -0.059 -0.027 -0.091  326.12
    "" -0.265 -0.226 -0.304  326.71
    ""  0.244  0.287  0.201  328.49
    "" -0.159 -0.118 -0.200  329.30
  1975  0.049  0.083  0.015  330.84
    "" -0.039 -0.004 -0.074  331.67
    ""  0.312  0.348  0.276  332.77
    ""  0.136  0.171  0.101  335.01
    ""  0.177  0.214  0.140  336.22
  1980  0.376  0.413  0.339  338.13
    ""  0.573  0.614  0.532  339.42
    ""  0.128  0.166  0.090  341.09
    ""  0.486  0.523  0.449  341.75
    ""  0.284  0.319  0.249  344.32
  1985  0.284  0.321  0.247  345.59
    ""  0.236  0.286  0.186  346.82
    ""  0.343  0.378  0.308  348.66
    ""  0.556  0.589  0.523  350.49
    ""  0.072  0.111  0.033  352.99
  1990  0.419  0.459  0.379  353.78
    ""  0.439  0.478  0.400  354.90
    ""  0.497  0.535  0.459  356.29
    ""  0.399  0.439  0.359  357.06
    ""  0.300  0.340  0.260  358.25
  1995  0.526  0.560  0.492  359.91
    ""  0.330  0.364  0.296  361.98
    ""  0.366  0.405  0.327  363.03
    ""  0.651  0.689  0.613  365.19
    ""  0.503  0.540  0.466  368.13
  2000  0.330  0.370  0.290  369.24
    ""  0.511  0.548  0.474  370.60
    ""  0.769  0.807  0.731  372.48
    ""  0.801  0.835  0.767  374.82
    ""  0.647  0.682  0.612  376.96
  2005  0.790  0.832  0.748  378.37
    ""  0.643  0.679  0.607  381.33
    ""  0.990  1.027  0.953  382.88
    ""  0.308  0.357  0.259  385.54
    ""  0.711  0.746  0.676  386.86
  2010  0.827  0.869  0.785  388.62
    ""  0.609  0.640  0.578  391.19
    ""  0.476  0.513  0.439  393.07
    ""  0.736  0.773  0.699  395.62
    ""  0.786  0.823  0.749  397.74
  2015  0.866  0.902  0.830  399.92
    ""  1.249  1.293  1.205  402.45
    ""  1.089  1.125  1.053  406.04
    ""  0.874  0.917  0.831  407.83
    ""  0.949  0.987  0.911  410.69
  2020  1.198  1.233  1.163  413.22
    ""  0.892  0.926  0.858  415.20
    ""  0.942  0.979  0.905  417.84
    ""  0.925  0.968  0.882  419.18

Footnote: https://github.com/soren-kragh/chartgen
FootnotePos: Right
)EOF";
