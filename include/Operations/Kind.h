//
//  Operations/Kind.h
//  ktool
//
//  Created by suhaspai on 11/15/22.
//

#pragma once

namespace Operations {
    enum class Kind {
        PrintHeader,
        PrintId,
        PrintLoadCommands,
        PrintLibraries,
        PrintArchs,
        PrintCStringSection,
        PrintSymbolPtrSection,
        PrintExportTrie,
        PrintBindOpcodeList,
        PrintBindActionList,
        PrintBindSymbolList,
        PrintRebaseOpcodeList,
        PrintRebaseActionList,
        PrintObjcClassList,
        PrintProgramTrie,
        PrintImageList
    };
}
