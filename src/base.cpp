//base.cpp
#include"pch.h"
#include"base.hpp"

namespace Send::Internal {


	Base::Base()
	{
	}




	// -------------------- find_device --------------------



}


namespace Send {
	std::unique_ptr<Send::Internal::Base> g_send = nullptr;
}