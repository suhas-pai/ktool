//
//  Operations/PrintObjcClassList.h
//  ktool
//
//  Created by suhaspai on 11/22/22.
//

#pragma once
#include <vector>

#include "ADT/Tree.h"
#include "MachO/BindInfo.h"
#include "Objects/DscImage.h"

#include "Base.h"

namespace Operations {
    struct PrintObjcClassList : public Base {
    public:
        struct Options {
            bool PrintCategories : 1 = false;
            bool PrintTree : 1 = false;
            bool Verbose : 1 = false;
            uint32_t TabLength = ADT::TreeNode::DefaultTabLength;

            enum class SortKind {
                None,
                ByName,
                ByDylibOrdinal,
                ByKind
            };

            std::vector<SortKind> SortKindList;
        };
    protected:
        FILE *OutFile;
        Options Opt;
    public:
        constexpr static auto Kind = Operations::Kind::PrintObjcClassList;
        explicit
        PrintObjcClassList(FILE *OutFile,
                           const struct Options &Options) noexcept;

        ~PrintObjcClassList() noexcept override {}

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                BindOpcodeParseError,

                NoDyldInfo,
                NoObjcData,
                UnalignedSection,
                ObjcDataOutOfBounds,
            };

            Error Error = Error::None;
            union {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wnested-anon-types"
                struct {
                    MachO::BindInfoKind BindKind;
                    MachO::BindOpcodeParseResult ParseResult;
                } BindOpcodeParseResult;
            #pragma GCC diagnostic pop
            };

            explicit RunResult(const enum Error Error) noexcept
            : Error(Error) {}

            explicit
            RunResult(const MachO::BindInfoKind Kind,
                      const MachO::BindOpcodeParseResult Result) noexcept
            : Error(Error::BindOpcodeParseError),
              BindOpcodeParseResult{Kind, Result} {}

            auto operator=(const RunResult &Other) noexcept -> decltype(*this) {
                Error = Other.Error;
                if (Error == Error::BindOpcodeParseError) {
                    BindOpcodeParseResult = Other.BindOpcodeParseResult;
                }

                return *this;
            }

            auto operator=(RunResult &&Other) noexcept -> decltype(*this) {
                Error = std::move(Other.Error);
                if (Error == Error::BindOpcodeParseError) {
                    BindOpcodeParseResult =
                        std::move(Other.BindOpcodeParseResult);
                }

                return *this;
            }

            inline RunResult(const RunResult &Other) noexcept {
                this->operator=(Other);
            }

            inline RunResult(RunResult &&Other) noexcept {
                this->operator=(std::move(Other));
            }

            ~RunResult() noexcept {
                if (Error == Error::BindOpcodeParseError) {
                    BindOpcodeParseResult.ParseResult.~BindOpcodeParseResult();
                }
            }
        };

        bool supportsObjectKind(Objects::Kind Kind) const noexcept override;

        RunResult run(const Objects::Base &Base) const noexcept;
        RunResult run(const Objects::MachO &MachO) const noexcept;
        RunResult run(const Objects::DscImage &Image) const noexcept;

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}