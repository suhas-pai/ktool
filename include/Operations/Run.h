//
//  Operations/Run.h
//  ktool
//
//  Created by suhaspai on 12/14/24.
//

#pragma once
#include "Objects/Base.h"

#include "PrintArchs.h"
#include "PrintBindActionList.h"
#include "PrintBindOpcodeList.h"
#include "PrintBindSymbolList.h"
#include "PrintCStringSection.h"
#include "PrintExportTrie.h"
#include "PrintHeader.h"
#include "PrintId.h"
#include "PrintImageList.h"
#include "PrintLibraries.h"
#include "PrintLoadCommands.h"
#include "PrintObjcClassList.h"
#include "PrintProgramTrie.h"
#include "PrintRebaseActionList.h"
#include "PrintRebaseOpcodeList.h"
#include "PrintSymbolPtrSection.h"

#include "Base.h"

namespace Operations {
    struct HandleFileOptions {
        std::string_view Path;

        int64_t ArchIndex = -1;
        int64_t ImageIndex = -1;

        Objects::DyldSharedCache::SubCacheProvidedPathMap
          SubCacheProvidedPathMap = {};

        explicit HandleFileOptions() noexcept {};
        explicit HandleFileOptions(std::string_view Path) noexcept
        : Path(Path) {}

        ~HandleFileOptions() noexcept {}
    };

    struct RunResult {
        Operations::Kind Kind;
        union {
            PrintHeader::RunResult PrintHeaderResult;
            PrintId::RunResult PrintIdResult;
            PrintLoadCommands::RunResult PrintLoadCommandsResult;
            PrintLibraries::RunResult PrintLibrariesResult;
            PrintArchs::RunResult PrintArchsResult;
            PrintCStringSection::RunResult PrintCStringSectionResult;
            PrintSymbolPtrSection::RunResult PrintSymbolPtrSectionResult;
            PrintExportTrie::RunResult PrintExportTrieResult;
            PrintBindOpcodeList::RunResult PrintBindOpcodeListResult;
            PrintBindActionList::RunResult PrintBindActionListResult;
            PrintBindSymbolList::RunResult PrintBindSymbolListResult;
            PrintRebaseOpcodeList::RunResult PrintRebaseOpcodeListResult;
            PrintRebaseActionList::RunResult PrintRebaseActionListResult;
            PrintObjcClassList::RunResult PrintObjcClassListResult;
            PrintProgramTrie::RunResult PrintProgramTrieResult;
            PrintImageList::RunResult PrintImageListResult;
        };

        explicit RunResult(const PrintHeader::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintHeader), PrintHeaderResult(Result) {}

        explicit RunResult(PrintHeader::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintHeader),
          PrintHeaderResult(std::move(Result)) {}

        explicit RunResult(const PrintId::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintId), PrintIdResult(Result) {}

        explicit RunResult(PrintId::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintId), PrintIdResult(std::move(Result)) {}

        explicit RunResult(const PrintLoadCommands::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintLoadCommands),
          PrintLoadCommandsResult(Result) {}

        explicit RunResult(PrintLoadCommands::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintLoadCommands),
          PrintLoadCommandsResult(std::move(Result)) {}

        explicit RunResult(const PrintLibraries::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintLibraries),
          PrintLibrariesResult(Result) {}

        explicit RunResult(PrintLibraries::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintLibraries),
          PrintLibrariesResult(std::move(Result)) {}

        explicit RunResult(const PrintArchs::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintArchs), PrintArchsResult(Result) {}

        explicit RunResult(PrintArchs::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintArchs),
          PrintArchsResult(std::move(Result)) {}

        explicit
        RunResult(const PrintCStringSection::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintHeader),
          PrintCStringSectionResult(Result) {}

        explicit
        RunResult(PrintCStringSection::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintHeader),
          PrintCStringSectionResult(std::move(Result)) {}

        explicit
        RunResult(const PrintSymbolPtrSection::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintSymbolPtrSection),
          PrintSymbolPtrSectionResult(Result) {}

        explicit
        RunResult(PrintSymbolPtrSection::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintSymbolPtrSection),
          PrintSymbolPtrSectionResult(std::move(Result)) {}

        explicit RunResult(const PrintExportTrie::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintExportTrie),
          PrintExportTrieResult(Result) {}

        explicit RunResult(PrintExportTrie::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintExportTrie),
          PrintExportTrieResult(std::move(Result)) {}

        explicit
        RunResult(const PrintBindOpcodeList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintBindOpcodeList),
          PrintBindOpcodeListResult(Result) {}

        explicit
        RunResult(PrintBindOpcodeList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintBindOpcodeList),
          PrintBindOpcodeListResult(std::move(Result)) {}

        explicit
        RunResult(const PrintBindActionList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintBindActionList),
          PrintBindActionListResult(Result) {}

        explicit
        RunResult(PrintBindActionList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintBindActionList),
          PrintBindActionListResult(std::move(Result)) {}

        explicit
        RunResult(const PrintBindSymbolList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintBindSymbolList),
          PrintBindSymbolListResult(Result) {}

        explicit
        RunResult(PrintBindSymbolList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintBindSymbolList),
          PrintBindSymbolListResult(std::move(Result)) {}

        explicit
        RunResult(const PrintRebaseOpcodeList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintBindOpcodeList),
          PrintRebaseOpcodeListResult(Result) {}

        explicit
        RunResult(PrintRebaseOpcodeList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintBindOpcodeList),
          PrintRebaseOpcodeListResult(std::move(Result)) {}

        explicit
        RunResult(const PrintRebaseActionList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintRebaseActionList),
          PrintRebaseActionListResult(Result) {}

        explicit
        RunResult(PrintRebaseActionList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintRebaseActionList),
          PrintRebaseActionListResult(std::move(Result)) {}

        explicit
        RunResult(const PrintObjcClassList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintObjcClassList),
          PrintObjcClassListResult(Result) {}

        explicit
        RunResult(PrintObjcClassList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintObjcClassList),
          PrintObjcClassListResult(std::move(Result)) {}

        explicit RunResult(const PrintProgramTrie::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintProgramTrie),
          PrintProgramTrieResult(Result) {}

        explicit RunResult(PrintProgramTrie::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintProgramTrie),
          PrintProgramTrieResult(std::move(Result)) {}

        explicit RunResult(const PrintImageList::RunResult &Result) noexcept
        : Kind(Operations::Kind::PrintImageList),
          PrintImageListResult(Result) {}

        explicit RunResult(PrintImageList::RunResult &&Result) noexcept
        : Kind(Operations::Kind::PrintImageList),
          PrintImageListResult(std::move(Result)) {}

        RunResult(const RunResult &Other) {
            switch (Other.Kind) {
                case Operations::Kind::PrintHeader:
                    PrintHeaderResult = Other.PrintHeaderResult;
                    break;
                case Operations::Kind::PrintId:
                    PrintIdResult = Other.PrintIdResult;
                    break;
                case Operations::Kind::PrintLoadCommands:
                    PrintLoadCommandsResult = Other.PrintLoadCommandsResult;
                    break;
                case Operations::Kind::PrintLibraries:
                    PrintLibrariesResult = Other.PrintLibrariesResult;
                    break;
                case Operations::Kind::PrintArchs:
                    PrintArchsResult = Other.PrintArchsResult;
                    break;
                case Operations::Kind::PrintCStringSection:
                    PrintCStringSectionResult = Other.PrintCStringSectionResult;
                    break;
                case Operations::Kind::PrintSymbolPtrSection:
                    PrintSymbolPtrSectionResult =
                        Other.PrintSymbolPtrSectionResult;
                    break;
                case Operations::Kind::PrintExportTrie:
                    PrintExportTrieResult = Other.PrintExportTrieResult;
                    break;
                case Operations::Kind::PrintBindOpcodeList:
                    PrintBindOpcodeListResult = Other.PrintBindOpcodeListResult;
                    break;
                case Operations::Kind::PrintBindActionList:
                    PrintBindActionListResult = Other.PrintBindActionListResult;
                    break;
                case Operations::Kind::PrintBindSymbolList:
                    PrintBindSymbolListResult = Other.PrintBindSymbolListResult;
                    break;
                case Operations::Kind::PrintRebaseOpcodeList:
                    PrintRebaseOpcodeListResult =
                        Other.PrintRebaseOpcodeListResult;
                    break;
                case Operations::Kind::PrintRebaseActionList:
                    PrintRebaseActionListResult =
                        Other.PrintRebaseActionListResult;
                    break;
                case Operations::Kind::PrintObjcClassList:
                    PrintObjcClassListResult = Other.PrintObjcClassListResult;
                    break;
                case Operations::Kind::PrintProgramTrie:
                    PrintProgramTrieResult = Other.PrintProgramTrieResult;
                    break;
                case Operations::Kind::PrintImageList:
                    PrintImageListResult = Other.PrintImageListResult;
                    break;
            }
        }

        RunResult(RunResult &&Other) {
            switch (Other.Kind) {
                case Operations::Kind::PrintHeader:
                    PrintHeaderResult = Other.PrintHeaderResult;
                    break;
                case Operations::Kind::PrintId:
                    PrintIdResult = Other.PrintIdResult;
                    break;
                case Operations::Kind::PrintLoadCommands:
                    PrintLoadCommandsResult = Other.PrintLoadCommandsResult;
                    break;
                case Operations::Kind::PrintLibraries:
                    PrintLibrariesResult = Other.PrintLibrariesResult;
                    break;
                case Operations::Kind::PrintArchs:
                    PrintArchsResult = Other.PrintArchsResult;
                    break;
                case Operations::Kind::PrintCStringSection:
                    PrintCStringSectionResult = Other.PrintCStringSectionResult;
                    break;
                case Operations::Kind::PrintSymbolPtrSection:
                    PrintSymbolPtrSectionResult =
                        Other.PrintSymbolPtrSectionResult;
                    break;
                case Operations::Kind::PrintExportTrie:
                    PrintExportTrieResult = Other.PrintExportTrieResult;
                    break;
                case Operations::Kind::PrintBindOpcodeList:
                    PrintBindOpcodeListResult = Other.PrintBindOpcodeListResult;
                    break;
                case Operations::Kind::PrintBindActionList:
                    PrintBindActionListResult = Other.PrintBindActionListResult;
                    break;
                case Operations::Kind::PrintBindSymbolList:
                    PrintBindSymbolListResult = Other.PrintBindSymbolListResult;
                    break;
                case Operations::Kind::PrintRebaseOpcodeList:
                    PrintRebaseOpcodeListResult =
                        Other.PrintRebaseOpcodeListResult;
                    break;
                case Operations::Kind::PrintRebaseActionList:
                    PrintRebaseActionListResult =
                        Other.PrintRebaseActionListResult;
                    break;
                case Operations::Kind::PrintObjcClassList:
                    PrintObjcClassListResult = Other.PrintObjcClassListResult;
                    break;
                case Operations::Kind::PrintProgramTrie:
                    PrintProgramTrieResult = Other.PrintProgramTrieResult;
                    break;
                case Operations::Kind::PrintImageList:
                    PrintImageListResult = Other.PrintImageListResult;
                    break;
            }
        }

        ~RunResult() noexcept {
            switch (Kind) {
                case Operations::Kind::PrintHeader:
                    PrintHeaderResult.~RunResult();
                    break;
                case Operations::Kind::PrintId:
                    PrintIdResult.~RunResult();
                    break;
                case Operations::Kind::PrintLoadCommands:
                    PrintLoadCommandsResult.~RunResult();
                    break;
                case Operations::Kind::PrintLibraries:
                    PrintLibrariesResult.~RunResult();
                    break;
                case Operations::Kind::PrintArchs:
                    PrintArchsResult.~RunResult();
                    break;
                case Operations::Kind::PrintCStringSection:
                    PrintCStringSectionResult.~RunResult();
                    break;
                case Operations::Kind::PrintSymbolPtrSection:
                    PrintSymbolPtrSectionResult.~RunResult();
                    break;
                case Operations::Kind::PrintExportTrie:
                    PrintExportTrieResult.~RunResult();
                    break;
                case Operations::Kind::PrintBindOpcodeList:
                    PrintBindOpcodeListResult.~RunResult();
                    break;
                case Operations::Kind::PrintBindActionList:
                    PrintBindActionListResult.~RunResult();
                    break;
                case Operations::Kind::PrintBindSymbolList:
                    PrintBindSymbolListResult.~RunResult();
                    break;
                case Operations::Kind::PrintRebaseOpcodeList:
                    PrintRebaseOpcodeListResult.~RunResult();
                    break;
                case Operations::Kind::PrintRebaseActionList:
                    PrintRebaseActionListResult.~RunResult();
                    break;
                case Operations::Kind::PrintObjcClassList:
                    PrintObjcClassListResult.~RunResult();
                    break;
                case Operations::Kind::PrintProgramTrie:
                    PrintProgramTrieResult.~RunResult();
                    break;
                case Operations::Kind::PrintImageList:
                    PrintImageListResult.~RunResult();
                    break;
            }
        }

        [[nodiscard]] auto isUnsupportedError() const noexcept -> bool;
    };

    auto Run(const Objects::Base &Base, const Operations::Base &Op)
        -> RunResult;

    auto
    RunAndHandleFile(const Operations::Base &Op,
                     const HandleFileOptions &Options) noexcept
        -> RunResult;

    auto
    RunAndHandleFile(const Operations::Base &Op,
                     const Objects::Base &Object,
                     const HandleFileOptions &Options) noexcept
        -> RunResult;
}