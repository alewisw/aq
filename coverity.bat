msbuild AQ.sln /t:Clean /p:Configuration=Release
cov-build --dir cov-int msbuild AQ.sln /t:Build /p:Configuration=Release
