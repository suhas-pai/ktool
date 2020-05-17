//
//  include/Operations/Kind.h
//  stool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

enum class OperationKind {
    None,

    // Print-Operations should have the LSB set.
    PrintHeader          = (1ull << 1) | 1,
    PrintLoadCommands    = (2ull << 1) | 1,
    PrintSharedLibraries = (3ull << 1) | 1,
    PrintId              = (4ull << 1) | 1,
    PrintArchList        = (5ull << 1) | 1,
    PrintFlags           = (6ull << 1) | 1,
    PrintExportTrie      = (7ull << 1) | 1,
    PrintObjcClassList   = (8ull << 1) | 1
};
