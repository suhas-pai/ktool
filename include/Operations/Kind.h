//
//  Operations/Kind.h
//  ktool
//
//  Created by Suhas Pai on 4/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

enum class OperationKind {
    None,

    PrintHeader           = (1ull << 1),
    PrintLoadCommands     = (2ull << 1),
    PrintSharedLibraries  = (3ull << 1),
    PrintId               = (4ull << 1),
    PrintArchList         = (5ull << 1),
    PrintExportTrie       = (6ull << 1),
    PrintObjcClassList    = (7ull << 1),
    PrintBindActionList   = (8ull << 1),
    PrintBindOpcodeList   = (9ull << 1),
    PrintBindSymbolList   = (10ull << 1),
    PrintRebaseActionList = (11ull << 1),
    PrintRebaseOpcodeList = (12ull << 1),
    PrintCStringSection   = (13ull << 1),
    PrintSymbolPtrSection = (14ull << 1),
    PrintImageList        = (15ull << 1)
};
