project(Core)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC inc)
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /EHsc>)

target_sources(${PROJECT_NAME} PRIVATE
	src/Algorithms/AStar.test.cpp
	src/Algorithms/Shoelace.test.cpp
	src/Algorithms/ShuntingYard.test.cpp

	src/Constexpr/ConstexprAlgorithms.cpp
	src/Constexpr/ConstexprBits.cpp
	src/Constexpr/ConstexprCollections.cpp
	src/Constexpr/ConstexprGeometry.cpp
	src/Constexpr/ConstexprMath.cpp
	src/Constexpr/ConstexprMatrix.cpp
	src/Constexpr/ConstexprRandom.cpp
	src/Constexpr/ConstexprStrUtils.cpp

	src/Instrumentation/ISink.cpp
	src/Instrumentation/Benchmark/ResourceMonitor_Windows.h
	src/Instrumentation/Benchmark/ResourceMonitor_Linux.h
	src/Instrumentation/Benchmark/ResourceMonitor.cpp
	src/Instrumentation/Benchmark/Stats.cpp
	src/Instrumentation/Logging.cpp
	src/Instrumentation/LogWriter/StdOutLogWriter.cpp

	src/Platform/ExecuteCommand_win.cpp

	src/Utilities/ExpressionEvaluator.cpp
	src/Utilities/FileUtils.cpp
	src/Utilities/PathUtils.cpp
	src/Utilities/ProgressBar.cpp
	src/Utilities/StringUtils.cpp
	src/Utilities/TimeUtils.cpp

	src/BigInt.cpp
	src/Concepts.tests.cpp
)