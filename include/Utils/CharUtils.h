//
//  include/Utils/CharUtils.h
//  stool
//
//  Created by Suhas Pai on 5/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

static inline bool IsDigit(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '0') < 10);
}

static inline bool IsControl(char Ch) noexcept {
    return (static_cast<unsigned>(Ch - '\0') < 32);
}

static inline bool IsSpace(char Ch) noexcept {
    switch (Ch) {
        case ' ':
        case '\n':
        case '\t':
        case '\v':
        case '\f':
        case '\r':
            return true;
    }

    return false;
}
