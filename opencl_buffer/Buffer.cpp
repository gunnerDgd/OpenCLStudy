#include <iostream>

#include <CL/cl.hpp>
#include <CL/cl_platform.h>

int main()
{
	cl::Platform	   CLPlatform;
	cl::Platform::get(&CLPlatform);

	std::vector<cl::Device>					   CLDeviceVector;
	CLPlatform.getDevices(CL_DEVICE_TYPE_GPU, &CLDeviceVector);

	cl::Device& CLGpuDevice = CLDeviceVector[0];
	cl::Buffer  CLGpuBuffer(CL_MEM_READ_WRITE, 256);
	
	cl_uint     CLGpuHostMemory[256];
	for (cl_uint i = 0; i < 256; i++)
		CLGpuHostMemory[i] = i;

	cl::Context		 CLGpuContext(CLGpuDevice);
	cl::CommandQueue CLGpuQueue  (CLGpuContext, CLGpuDevice);

	CLGpuQueue.enqueueWriteBuffer(CLGpuBuffer, true, 0, 256, CLGpuHostMemory);
}