#include <CL/cl.hpp>
#include <CL/cl_platform.h>

#include <iostream>

int main()
{
	std::vector<cl::Platform> CLPlatformVector;
	cl::Platform::get		(&CLPlatformVector);

	for (auto&& Platform : CLPlatformVector)
	{
		std::vector<cl::Device> CLDeviceVector;
		Platform.getDevices(CL_DEVICE_TYPE_ALL, &CLDeviceVector);

		for (auto&& Device : CLDeviceVector)
			std::cout << Device.getInfo<CL_DEVICE_NAME>() << std::endl;
	}
}