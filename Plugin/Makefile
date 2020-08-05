
updateCMake:
	@(../ThirdParty/FRUT/prefix/FRUT/bin/Jucer2CMake reprojucer funFilter.jucer ../ThirdParty/FRUT/prefix/FRUT/cmake/Reprojucer.cmake)

format:
	@(clang-format -i -style=file Source/*)
	@(xmllint -format --output FunFilter.jucer FunFilter.jucer)
