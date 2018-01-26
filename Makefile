
all:
	MONO_IOMAP=all msbuild ReedAndSheppCS/ReedAndSheppCS.csproj

old:
	MONO_IOMAP=all xbuild ReedAndSheppCS/ReedAndSheppCS.csproj
