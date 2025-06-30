#include <hip/hip_runtime.h>

#define HIP_ERRCHK(result) hip_errchk(result, __FILE__, __LINE__)
static inline void hip_errchk(hipError_t result, const char *file, int32_t line) {
    if (result != hipSuccess) {
        printf("\n\n%s in %s at line %d\n", hipGetErrorString(result), file, line);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    // Wrap any API calls with the HIP_ERRCHK macro
    // Then, compile and run the program and fix any errors you encounter.
    // Repeat, until you've fixed all the errors.
    // You can consult the HIP API documentation for the correct way to call
    // each function:
    // https://rocm.docs.amd.com/projects/HIP/en/latest/reference/hip_runtime_api/modules.html#modules-reference

    int count;
    HIP_ERRCHK(hipGetDeviceCount(&count));

    int device;
    HIP_ERRCHK(hipGetDevice(&device));

    // Can only set device between [0, DeviceCount-1]
    HIP_ERRCHK(hipSetDevice(0));

    void *ptr = nullptr;
    // Must a pointer to store
    int value;
    HIP_ERRCHK(hipDeviceGetAttribute(&value, hipDeviceAttribute_t::hipDeviceAttributeTotalGlobalMem, device));
    printf("Total device memory %d\n", value);

    HIP_ERRCHK(hipMalloc(&ptr, 2147483647));

    HIP_ERRCHK(hipMemset(ptr, 0, 8));

    HIP_ERRCHK(hipFree(ptr));

    return 0;
}
