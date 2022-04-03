#pragma once
#include "pch.h"
class Thread
{
	virtual void open();
	virtual void start();
	virtual void close();
	virtual void main();
};

