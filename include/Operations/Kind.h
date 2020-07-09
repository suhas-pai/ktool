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

    PrintHeader           = (1ull << 1),
    PrintLoadCommands     = (2ull << 1),
    PrintSharedLibraries  = (3ull << 1),
    PrintId               = (4ull << 1),
    PrintArchList         = (5ull << 1),
    PrintFlags            = (6ull << 1),
    PrintExportTrie       = (7ull << 1),
    PrintObjcClassList    = (8ull << 1),
    PrintBindActionList   = (9ull << 1),
    PrintBindOpcodeList   = (10ull << 1),
    PrintBindSymbolList   = (11ull << 1),
    PrintRebaseActionList = (12ull << 1),
    PrintRebaseOpcodeList = (13ull << 1),
    PrintCStringSection   = (14ull << 1),
    PrintSymbolPtrSection = (15ull << 1)
};
