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
    PrintHeader,
    PrintLoadCommands,
    PrintSharedLibraries,
    PrintId,
    PrintArchList,
    PrintFlags,
    PrintExportTrie
};
