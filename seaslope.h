#pragma once

#ifdef SEASLOPE_EXPORTS 
#define SEASLOPE_API _declspec(dllexport)
#else 
#define SEASLOPE_API _declspec(dllimport)
#endif

namespace move
{
	struct SEASLOPE_API DATA
	{
		float slope = 0;
		float base = 0;
	};

	extern SEASLOPE_API DATA Init(float StartX, float StartY, float EndX, float EndY);
	extern SEASLOPE_API float NextY(float nextX, DATA LineData);

}
