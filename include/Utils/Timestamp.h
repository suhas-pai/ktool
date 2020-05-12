//
//  include/Utils/Timestamp.h
//  stool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <ctime>
#include <string>

std::string GetHumanReadableTimestamp(time_t Timestamp) noexcept;
