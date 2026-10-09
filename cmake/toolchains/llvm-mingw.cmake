# Windows x64 with llvm-mingw (clang + lld + libc++ + mingw-w64, UCRT). The runtime and the
# recompiled game need clang (ext_vector_type, __builtin_elementwise_*), and llvm-mingw can be
# redistributed, so the app ships a trimmed copy to compile the game on the player's PC.
# Point LLVM_MINGW_ROOT (cache or environment) at the unpacked release, or have its bin on PATH.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

if(NOT LLVM_MINGW_ROOT AND DEFINED ENV{LLVM_MINGW_ROOT})
    set(LLVM_MINGW_ROOT "$ENV{LLVM_MINGW_ROOT}")
endif()
set(_rt_triple x86_64-w64-mingw32)
if(LLVM_MINGW_ROOT)
    # Windows paths arrive with backslashes (Python's Path, %VAR%); CMake writes them into generated
    # files inside quoted strings, where a backslash followed by a letter is an invalid escape: use
    # forward slashes only.
    file(TO_CMAKE_PATH "${LLVM_MINGW_ROOT}" LLVM_MINGW_ROOT)
    set(LLVM_MINGW_ROOT "${LLVM_MINGW_ROOT}" CACHE PATH "llvm-mingw directory" FORCE)
    set(_rt_bin "${LLVM_MINGW_ROOT}/bin")
    set(CMAKE_C_COMPILER   "${_rt_bin}/${_rt_triple}-clang.exe")
    set(CMAKE_CXX_COMPILER "${_rt_bin}/${_rt_triple}-clang++.exe")
    set(CMAKE_RC_COMPILER  "${_rt_bin}/llvm-windres.exe")
    set(CMAKE_AR           "${_rt_bin}/llvm-ar.exe")
    set(CMAKE_RANLIB       "${_rt_bin}/llvm-ranlib.exe")
else()
    set(CMAKE_C_COMPILER   ${_rt_triple}-clang)
    set(CMAKE_CXX_COMPILER ${_rt_triple}-clang++)
    set(CMAKE_RC_COMPILER  llvm-windres)
endif()

# SSE4.1 is the floor (_mm_extract_epi32/64); the game's flags.json inherits this, so no AVX2.
set(CMAKE_C_FLAGS_INIT   "-march=x86-64-v2")
set(CMAKE_CXX_FLAGS_INIT "-march=x86-64-v2 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "-fuse-ld=lld -stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld -stdlib=libc++")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=lld -stdlib=libc++")
