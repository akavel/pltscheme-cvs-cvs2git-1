cd fit-code
sh make_fit_objects.sh
cd ..
mzc ++ccf "/I./fit-code" ++ldf fit-code/matrix.obj ++ldf fit-code/fit.obj  fit-low-level.ss
mv fit-low-level.dll compiled/native/win32/i386/