//
//  include/Utils/Timestamp.h
//  ktool
//
//  Created by Suhas Pai on 4/24/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <ctime>
#include <string>

[[nodiscard]] std::string GetHumanReadableTimestamp(time_t Timestamp) noexcept;
