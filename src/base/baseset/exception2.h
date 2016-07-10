
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "exception_global.h"
#include <exception>

namespace exception{
	class not_impl : public std::exception{
		not_impl() : std::exception("this function is not implement yet.") {}
	};
}

#endif // EXCEPTION_H
