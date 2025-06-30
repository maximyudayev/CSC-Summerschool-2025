#include <hip/hip_runtime.h>

#include "error_checking.hpp"

__global__ void hello() {
    if (threadIdx.x == 0 && threadIdx.y == 0 && threadIdx.z == 0 &&
        blockIdx.x == 0 && blockIdx.y == 0 && blockIdx.z == 0) {
        printf("You fixed me! Hooray!\n");
    }
}

int main(int argc, char **argv) {
    // After completing the error checking in the separate file
    // fix any errors with this launch.
    // To do that,
    // - compile the program
    // - run it
    // - fix any errors with the launch parameters
    // - repeat 1025 30000 1
    dim3 blockIdx = dim3(1025,30,1);
    dim3 threadIdx = dim3(1,1024,1);
    LAUNCH_KERNEL(hello, blockIdx, threadIdx, 65536, 0);
    [[maybe_unused]] const auto result = hipDeviceSynchronize();

    return 0;
}
