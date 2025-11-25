set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10)

set(TRIPLE x86_64-w64-mingw32)

set(CMAKE_C_COMPILER   ${TRIPLE}-gcc)
set(CMAKE_CXX_COMPILER ${TRIPLE}-g++)
set(CMAKE_RC_COMPILER  ${TRIPLE}-windres)

# Search only here:
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib)

# Prevent finding Linux SDL
set(CMAKE_IGNORE_PATH "/usr/local/include" "/usr/include" "/usr/lib" "/usr/local/lib")

# Disable pkg-config (it returns Linux libs)
set(PKG_CONFIG_EXECUTABLE "")
