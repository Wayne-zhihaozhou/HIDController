//base.hpp
#pragma once




namespace Send::Internal {

	class Base {
	protected:

		//void mouse_absolute_to_screen(POINT& absolute) const;
		//void mouse_virtual_desk_absolute_to_screen(POINT& absolute) const;
		//static void mouse_screen_to_relative(POINT& screen_point);

	public:
		Base();
		virtual ~Base() = default;
		virtual void destroy() = 0;

	};


	//std::wstring find_device(std::function<bool(std::wstring_view name)> p);
}


namespace Send {
	extern std::unique_ptr<Send::Internal::Base> g_send;
}