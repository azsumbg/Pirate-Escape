#pragma once

#ifdef SEAFACTORY_EXPORTS 
#define SEAFACTORY_API _declspec(dllexport)
#else 
#define SEAFACTORY_API _declspec(dllimport)
#endif

#include <cstdlib>
#include <ctime>

constexpr float scr_width = 1000.0f;
constexpr float  scr_height = 800.0f;

constexpr int D_OK = 13001;
constexpr int D_FAIL = 13002;
constexpr int D_UNSUPPORTED = 13003;

enum class dirs { stop = 0, left = 1, right = 2, up = 3, down = 4 };
enum class types { no_type = 0, hero = 1, bad1 = 2, bad2 = 3, bad3 = 4, explosion = 5 };

namespace dll
{
	
	class SEAFACTORY_API ATOM
	{
	protected:
		float my_width = 0;
		float my_height = 0;

	public:
		dirs dir = dirs::stop;
		float x = 0;
		float y = 0;
		float ex = 0;
		float ey = 0;

		ATOM(float _x, float _y, float _width, float _height)
		{
			x = _x;
			y = _y;
			my_width = _width;
			my_height = _height;
			ex = x + my_width;
			ey = y + my_height;
		}
		virtual ~ATOM() {};

		float GetWidth()const
		{
			return my_width;
		}
		void SetWidth(float _new_width)
		{
			my_width = _new_width;
			ex = x + my_width;
		}

		float GetHeight() const
		{
			return my_height;
		}
		void SetHeight(float _new_height)
		{
			my_height = _new_height;
			ey = y + my_height;
		}

		void SetEdges()
		{
			ex = x + my_width;
			ey = y + my_width;
		}
		void NewDims(float _new_width, float _new_height)
		{
			my_width = _new_width;
			my_height = _new_height;
			ex = x + my_width;
			ey = y + my_height;
		}
	};

	class SEAFACTORY_API FACTORY : public ATOM
	{
	protected:
		int frame = 0;
		int frame_delay = 0;
		int max_frames = 0;

		int turn_delay = 0;
		int shoot_delay = 0;

		int range = 0;

		float speed = 1.0f;

	public:
		types type = types::no_type;
		int lifes = 0;
		bool now_shooting = false;


		FACTORY(types _what_type, float _x, float _y) :ATOM(_x, _y, 150.0f, 143.0f) //HERO by default;
		{
			type = _what_type;
			
			frame_delay = 5;
			max_frames = 6;
			turn_delay = 100;
			shoot_delay = 100;
			range = 150;
			lifes = 200;

			switch (type)
			{
			case types::bad1:
				NewDims(100.0f, 108.0f);
				frame_delay = 10;
				max_frames = 2;
				turn_delay = 80;
				shoot_delay = 90;
				speed = 1.2f;
				range = 120;
				lifes = 180;
				break;

			case types::bad2:
				NewDims(120.0f, 64.0f);
				frame_delay = 8;
				max_frames = 4;
				turn_delay = 90;
				shoot_delay = 100;
				speed = 1.0f;
				range = 130;
				lifes = 120;
				break;

			case types::bad3:
				NewDims(100.0f, 84.0f);
				frame_delay = 3;
				max_frames = 9;
				turn_delay = 110;
				shoot_delay = 120;
				speed = 0.8f;
				range = 130;
				lifes = 180;
				break;

			case types::explosion:
				NewDims(200.0f, 227.0f);
				frame_delay = 2;
				max_frames = 23;
				turn_delay = 0;
				shoot_delay = 0;
				speed = 0;
				break;
			}
		}

		virtual ~FACTORY() {};

		virtual int GetFrame() = 0;
		
		virtual void Release() = 0;
		virtual int Move(float _what_speed) = 0;
		virtual int Shoot() = 0;
	};
	
	/////////////////////////////////////////////
	
	struct SEAFACTORY_API FIELDDATA
	{
		float x = 0;
		float y = 0;
		float ex = 0;
		float ey = 0;
	};

	struct SEAFACTORY_API SCREENDATA
	{
		int number = -1;
		int up = -1;
		int down = -1;
		int left = -1;
		int right = -1;

		FIELDDATA Island1, Island2, Island3, FinalIsland;
	};
	
	struct SEAFACTORY_API BOULDER
	{
		ATOM Dims;
		int range = 0;
	};

	/// /////////////////////////////////////////
	
	typedef FACTORY* obj_ptr;
	typedef BOULDER* Boulder;

	extern SEAFACTORY_API obj_ptr iFactory(types _what_type, float _x, float _y, dirs _dir);

	extern SEAFACTORY_API void InitScreenData(SCREENDATA (&Screens)[8]);
}
