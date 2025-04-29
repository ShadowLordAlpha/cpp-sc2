message(STATUS "FetchContent: protocol")

FetchContent_Declare(
    sc2protocol
    GIT_REPOSITORY https://github.com/Blizzard/s2client-proto.git
    GIT_TAG a6d60f4cce7886a3be2bf7af2c1b12fdab6e9c63
)
FetchContent_MakeAvailable(sc2protocol)
