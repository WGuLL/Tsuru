
frutInstall:
	@(cd ../ThirdParty/FRUT && mkdir -p build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX="../prefix" -DJUCE_ROOT="../../JUCE" && cmake --build . --target install --parallel)

updateCMake:
	@(../ThirdParty/FRUT/prefix/FRUT/bin/Jucer2CMake reprojucer funFilter.jucer ../ThirdParty/FRUT/prefix/FRUT/cmake/Reprojucer.cmake)

format:
	@(clang-format -i -style=file Source/*)
	@(xmllint -format --output FunFilter.jucer FunFilter.jucer)
