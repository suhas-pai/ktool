/*
 * Operations/PrintBindSymbolList.h
 * © suhas pai
 */

#pragma once

#include <vector>

#include "MachO/BindInfo.h"
#include "Operations/Base.h"
#include "Objects/MachO.h"

namespace Operations {
    struct PrintBindSymbolList : public Base {
    public:
        struct Options {
            bool PrintNormal : 1 = true;
            bool PrintLazy : 1 = true;
            bool PrintWeak : 1 = true;
            bool Verbose : 1 = false;

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
        constexpr static auto Kind = Operations::Kind::PrintBindSymbolList;
        explicit
        PrintBindSymbolList(FILE *OutFile,
                            const struct Options &Options) noexcept;

        ~PrintBindSymbolList() noexcept override {}

        struct RunResult {
            enum class Error : uint32_t {
                None,
                Unsupported,

                NoDyldInfo,
                NoSymbols,

                BindOpcodeParseError
            };

            Error Error = Error::None;
            union {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wnested-anon-types"
                struct {
                    MachO::BindInfoKind BindKind;
                    MachO::BindOpcodeParseResult ParseResult;
                } BindOpcodeParseResult;
            #pragma clang diagnostic pop
            };

            explicit RunResult() noexcept {};
            explicit RunResult(const enum Error Error) noexcept
            : Error(Error) {}

            explicit
            RunResult(const MachO::BindInfoKind BindKind,
                      const MachO::BindOpcodeParseResult &ParseResult) noexcept
            : Error(Error::BindOpcodeParseError),
              BindOpcodeParseResult{BindKind, ParseResult} {}

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

        [[nodiscard]] constexpr auto &options() const noexcept {
            return Opt;
        }
    };
}