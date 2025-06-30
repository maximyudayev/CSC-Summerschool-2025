#include <cstdio>
#include <hip/hip_runtime.h>

// This file include macros for checking the API and kernel launch errors
#include "../../../error_checking.hpp"

__global__ void fill(float *arr, float a, size_t num_values) {
    // Fill the array 'arr' with the constant 'a'.
    int ind = threadIdx.x + blockIdx.x*blockDim.x;
    if (ind < num_values)
        arr[ind] = a;
    // Assume the array size is 'num_values'
    // Consult earlier exercises where we launched kernels and the lecture
    // slides for help
}

int main() {
    static constexpr size_t num_values = 1000000;
    static constexpr size_t num_bytes = sizeof(float) * num_values;
    static constexpr float a = 3.4f;

    float *d_arr = nullptr;
    // Allocate memory on the GPU
    HIP_ERRCHK(hipMalloc(&d_arr, num_bytes));

    // Define grid dimensions + launch the device kernel
    int threads = 1024;
    int blocks = (num_values+1024-1)/1024;
    LAUNCH_KERNEL(fill, blocks, threads, 0, 0, d_arr, a, num_values);

    float *h_arr = static_cast<float *>(std::malloc(num_bytes));
    // Copy results back to CPU
    HIP_ERRCHK(hipMemcpy(h_arr, d_arr, num_bytes, hipMemcpyDeviceToHost));

    // Free device memory
    HIP_ERRCHK(hipFree(d_arr));

    printf("Some values copied from the GPU: %f, %f, %f, %f\n", h_arr[0],
           h_arr[1], h_arr[num_values - 2], h_arr[num_values - 1]);

    float error = 0.0;
    static constexpr float tolerance = 1e-6f;
    for (size_t i = 0; i < num_values; i++) {
        const auto diff = abs(h_arr[i] - a);
        if (diff > tolerance) {
            error += diff;
        }
    }
    printf("total error: %f\n", error);
    printf("  reference: %f\n", a);
    printf("     result: %f at (42)\n", h_arr[42]);

    std::free(h_arr);

    HIP_ERRCHK(hipDeviceSynchronize());

    return 0;
}
