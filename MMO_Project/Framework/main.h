#pragma once
#include "pch.h"

static const int eThreadCount = 4;

#define F_DECLARE(pclass) \
using Shdptr\\pclass = std::shared_ptr<pclass> \