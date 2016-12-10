cmake_minimum_required(VERSION 3.3)
project(ParallelHW4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -Wall")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
set(SOURCE_FILES hw4/firewallImpl.c hw4/firewallImpl.h lib/crc32.h lib/crc32.c lib/fingerprint.c lib/fingerprint.h hw4/firewall.c lib/generators.c lib/generators.h lib/hashgenerator.c lib/hashgenerator.h lib/hashpacketworker.c lib/hashpacketworker.h lib/hashtable.c lib/hashtable.h lib/packetsource.c lib/packetsource.h lib/paddedprim.c lib/paddedprim.h lib/statistics.c lib/statistics.h lib/stopwatch.c lib/stopwatch.h hw4/queue.c hw4/queue.h hw4/locks.c hw4/locks.h lib/concurrenthashtable.c lib/concurrenthashtable.h lib/concurrentList.c lib/concurrentList.h lib/coarsehashtable.c lib/coarsehashtable.h hw4/serialLinkedList.c hw4/serialLinkedList.h)
add_executable(ParallelHW4 ${SOURCE_FILES})


TARGET_LINK_LIBRARIES(
        ParallelHW4 pthread m

)
set(SOURCE_FILES_TEST hw4/firewallImpl.c hw4/firewallImpl.h lib/crc32.h lib/crc32.c lib/fingerprint.c lib/fingerprint.h lib/generators.c lib/generators.h lib/hashgenerator.c lib/hashgenerator.h lib/hashpacketworker.c lib/hashpacketworker.h lib/hashtable.c lib/hashtable.h lib/packetsource.c lib/packetsource.h lib/paddedprim.c lib/paddedprim.h lib/statistics.c lib/statistics.h lib/stopwatch.c lib/stopwatch.h hw4/queue.c hw4/queue.h hw4/test.c hw4/locks.c hw4/locks.h lib/concurrenthashtable.c lib/concurrenthashtable.h lib/concurrentList.c lib/concurrentList.h lib/coarsehashtable.c lib/coarsehashtable.h hw4/serialLinkedList.c hw4/serialLinkedList.h)
add_executable(ParallelHW4Test ${SOURCE_FILES_TEST})


TARGET_LINK_LIBRARIES(
        ParallelHW4Test pthread m check rt

)