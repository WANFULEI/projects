/*!
 * \file exception2.h
 * \date 2016/06/24 19:39
 *
 * \author ÎºÇå
 * Contact: 897810981@qq.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
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
