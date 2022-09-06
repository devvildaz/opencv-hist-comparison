cc_library(
	name = "histo-lib",
	srcs = ["histogram/histogram.cpp"],
	hdrs = ["histogram/histogram.hpp"],
	includes = ["histogram"],
	deps = [
		"@opencv//:opencv"
	]
)

cc_binary(
  name = "opencv-main",
  srcs = ["main.cpp"],
  deps = [
		"@opencv//:opencv",
		":histo-lib"
	],
	data = ["//images:bcp.jpg"]
)
