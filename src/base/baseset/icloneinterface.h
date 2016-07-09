#pragma once

class ICloneInterface
{
	public:
		virtual ICloneInterface *clone() = 0;
}