#pragma once

#include <unistd.h>

#include "unique_resource.h"
#include "resource_traits.h"


using FdHolder = UniqueResource<ResourceTraits<int, -1, ::close>>;
