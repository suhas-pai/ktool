//
//  include/ADT/MachO.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <bitset>
#include <optional>

#include "Utils/Casting.h"
#include "Utils/DoesOverflow.h"
#include "Utils/SwitchEndian.h"

#include "BasicContinguousList.h"
#include "BasicMasksHandler.h"

#include "Mach-O/Headers.h"
#include "Mach-O/LoadCommands.h"
#include "Mach-O/LoadCommandTemplates.h"
#include "Mach-O/ObjC.h"
#include "Mach-O/SegmentInfo.h"
#include "Mach-O/SegmentUtil.h"
#include "Mach-O/Types.h"

#include "Mach.h"
#include "PointerOrError.h"
