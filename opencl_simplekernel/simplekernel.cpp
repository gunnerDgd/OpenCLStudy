#include <CL/cl.hpp>
#include <CL/cl_platform.h>

#include <iostream>
#include <Windows.h>

#define CLGpuProgramSource L".\\simplekernel.cl"

int main()
{
	cl::Platform	   CLPlatform;
	cl::Platform::get(&CLPlatform);

	std::vector<cl::Device>					   CLDeviceVector;
	CLPlatform.getDevices(CL_DEVICE_TYPE_GPU, &CLDeviceVector);

	cl::Device&		 CLGpuDevice = CLDeviceVector[0];
	cl::Context		 CLGpuContext (CLGpuDevice);
	cl::CommandQueue CLGpuQueue   (CLGpuContext, CLGpuDevice);
	
	HANDLE		 CLGpuProgramFile						   = ::CreateFile (CLGpuProgramSource, GENERIC_ALL, 0, 0, OPEN_EXISTING, 0, 0);
	DWORD		 CLGpuProgramSizeHigh, CLGpuProgramSizeLow = ::GetFileSize(CLGpuProgramFile, &CLGpuProgramSizeHigh);

	HANDLE		 CLGpuProgramFileMap =				::CreateFileMapping(CLGpuProgramFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	const char*  CLGpuProgramPointer = (const char*)::MapViewOfFile    (CLGpuProgramFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	cl::Program::Sources CLGpuSourceHandle(1, std::make_pair(CLGpuProgramPointer, (CLGpuProgramSizeHigh << 32) | CLGpuProgramSizeLow));
	cl::Program			 CLGpuProgram     (CLGpuContext, CLGpuSourceHandle);

	CLGpuProgram.build();
	cl::Kernel CLGpuKernel(CLGpuProgram, "CLSimpleKernel");

	std::unique_ptr<int[]> CLGpuHostInputArray1(new int[16]), CLGpuHostInputArray2(new int[16]), CLGpuHostOutputArray(new int[16]);
	cl::Buffer			   CLGpuMemoryInput1   (CLGpuContext, CL_MEM_READ_ONLY , sizeof(int) * 16),
						   CLGpuMemoryInput2   (CLGpuContext, CL_MEM_READ_ONLY , sizeof(int) * 16),
						   CLGpuMemoryOutput   (CLGpuContext, CL_MEM_WRITE_ONLY, sizeof(int) * 16);

	for (int CLGpuHostInit = 0; CLGpuHostInit < 16; CLGpuHostInit++)
	{
		CLGpuHostInputArray1[CLGpuHostInit] = CLGpuHostInit;
		CLGpuHostInputArray2[CLGpuHostInit] = CLGpuHostInit;
		CLGpuHostOutputArray[CLGpuHostInit] = CLGpuHostInit;
	}

	CLGpuQueue.enqueueWriteBuffer(CLGpuMemoryInput1, true, 0, 16 * sizeof(int), (const void*)CLGpuHostInputArray1.get());
	CLGpuQueue.enqueueWriteBuffer(CLGpuMemoryInput2, true, 0, 16 * sizeof(int), (const void*)CLGpuHostInputArray2.get());

	CLGpuKernel.setArg(0, CLGpuMemoryInput1);
	CLGpuKernel.setArg(1, CLGpuMemoryInput2);
	CLGpuKernel.setArg(2, CLGpuMemoryOutput);

	cl::NDRange						CLGpuTaskRange(1);
	CLGpuQueue.enqueueNDRangeKernel(CLGpuKernel, cl::NullRange, CLGpuTaskRange);
	CLGpuQueue.enqueueReadBuffer   (CLGpuMemoryOutput, true, 0, sizeof(int) * 16, CLGpuHostOutputArray.get());

	for (int CLGpuReadIt = 0; CLGpuReadIt < 16; CLGpuReadIt++)
		std::cout << (CLGpuHostOutputArray.get())[CLGpuReadIt] << " ";
}