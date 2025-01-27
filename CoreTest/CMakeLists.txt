project(CoreTest)

add_executable(${PROJECT_NAME} )

target_link_libraries(${PROJECT_NAME} gtest)
target_link_libraries(${PROJECT_NAME} gmock)
target_link_libraries(${PROJECT_NAME} Core)

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /EHsc>)

target_sources(${PROJECT_NAME} PRIVATE 
	src/Main.cpp
	src/DesignPatterns/Crtp.Test.cpp
	src/DesignPatterns/Mixin.test.cpp
	src/DesignPatterns/PubSub.test.cpp
	src/DesignPatterns/ServiceLocator.test.cpp

	src/Instrumentation/Logging.test.cpp
	src/Instrumentation/Benchmark/Stats.test.cpp
	src/Instrumentation/Benchmark/ResourceMonitor.test.cpp

	src/Macros/PreProcessorOverride.test.cpp

	src/Threading/Tasks.test.cpp

	src/Utilities/ConstexprCounter.test.cpp
	src/Utilities/ExpressionEvaluator.test.cpp
	src/Utilities/ExtendableEnum.test.cpp
	src/Utilities/Format.test.cpp
	src/Utilities/LambdaUtils.test.cpp
	src/Utilities/PathUtils.test.cpp
	src/Utilities/Require.test.cpp
	src/Utilities/StringUtils.test.cpp
	src/Utilities/TimeUtils.test.cpp
)