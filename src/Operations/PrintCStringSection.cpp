//
//  Operations/PrintCStringSection.cpp
//  ktool
//
//  Created by suhaspai on 11/21/22.
//

#include <algorithm>
#include <optional>
#include <vector>

#include "ADT/Maximizer.h"
#include "MachO/LoadCommands.h"

#include "Operations/Base.h"
#include "Operations/PrintCStringSection.h"

#include "Utils/Misc.h"
#include "Utils/Print.h"

namespace Operations {
    PrintCStringSection::PrintCStringSection(
        FILE *const OutFile,
        const std::optional<std::string> &SegmentName,
        const std::string &SectionName,
        const struct Options &Options) noexcept
    : OutFile(OutFile), Opt(Options), SegmentName(SegmentName),
      SectionName(SectionName) {}

    bool
    PrintCStringSection::supportsObjectKind(
        const Objects::Kind Kind) const noexcept
    {
        switch (Kind) {
            case Objects::Kind::None:
                assert(false &&
                       "Got Object-Kind None in "
                       "PrintCStringSection::supportsObjectKind");
            case Objects::Kind::MachO:
                return true;
            case Objects::Kind::FatMachO:
                return false;
        }

        assert(false &&
               "Got unknown Object-Kind in "
               "PrintCStringSection::supportsObjectKind");
    }

    [[nodiscard]]
    static auto ShouldExcludeString(std::string &String) noexcept {
        if (String.empty()) {
            return true;
        }

        for (auto Iter = String.begin(); Iter != String.end(); Iter++) {
            const auto Ch = *Iter;
            if (isprint(Ch) == 0) {
                switch (Ch) {
                    case '\v':
                        String.replace(Iter, Iter + 1, "\\v");
                        break;
                    case '\f':
                        String.replace(Iter, Iter + 1, "\\f");
                        break;
                    case '\r':
                        String.replace(Iter, Iter + 1, "\\r");
                        break;
                    case '\t':
                        String.replace(Iter, Iter + 1, "\\t");
                        break;
                    case '\n':
                        String.replace(Iter, Iter + 1, "\\n");
                        break;
                    default:
                        return true;
                }

                Iter++;
            }
        }

        return false;
    }

    struct CStringInfo {
        std::string String;

        uint64_t FileOffset;
        uint64_t Address;
    };

    static auto
    HandleCStringSection(const char *const SectData,
                         const uint64_t SectFileOff,
                         const uint64_t SectVmAddr,
                         const uint64_t SectSize,
                         const uint64_t Limit,
                         uint64_t &LongestCStringLength) noexcept
    {
        auto Result = std::vector<CStringInfo>();
        auto Length = uint64_t();
        auto LongestLength = ADT::Maximizer<uint64_t>();
        auto StringCount = uint64_t();

        for (auto Pos = uint64_t();
             StringCount < Limit && Pos < SectSize;
             Pos += Length + 1)
        {
            const auto StringPtr = SectData + Pos;
            Length = strnlen(StringPtr, SectSize);

            auto String = std::string(StringPtr, Length);
            if (ShouldExcludeString(String)) {
                continue;
            }

            const auto Info = CStringInfo {
                .String = std::move(String),
                .FileOffset = SectFileOff + Pos,
                .Address = SectVmAddr + Pos
            };

            LongestLength.set(Info.String.length());
            Result.emplace_back(std::move(Info));

            StringCount++;
        }

        LongestCStringLength = LongestLength.value();
        return Result;
    }

    template <MachO::LoadCommandDerived T>
    auto
    IterateSections(const T &Segment,
                    const std::string_view SectionName,
                    const bool IsBigEndian,
                    RunResult &Result,
                    const typename T::Section *&SectionOut) noexcept -> int
    {
        for (const auto &Section : Segment.sectionList(IsBigEndian)) {
            if (Section.sectionName() != SectionName) {
                continue;
            }

            if (Section.kind(IsBigEndian) != T::Section::Kind::CStringLiterals)
            {
                Result.set(PrintCStringSection::RunError::NotCStringSection);
                return 1;
            }

            SectionOut = &Section;
            return 0;
        }

        return 0;
    }

    auto
    PrintCStringSection::run(const Objects::MachO &MachO) const noexcept
        -> RunResult
    {
        auto Result = RunResult(Objects::Kind::MachO);
        if (SectionName.empty()) {
            return Result.set(RunError::EmptySectionName);
        }

        auto SectionData = static_cast<const char *>(nullptr);
        auto SectionFileOff = uint64_t();
        auto SectionAddr = uint64_t();
        auto SectionSize = uint64_t();

        const auto IsBigEndian = MachO.isBigEndian();
        const auto Is64Bit = MachO.is64Bit();

        if (Is64Bit) {
            for (const auto &LC : MachO.loadCommandsMap()) {
                using namespace MachO;
                if (const auto Segment =
                        dyn_cast<LoadCommandKind::Segment64>(&LC, IsBigEndian))
                {
                    if (const auto SegName = SegmentName) {
                        if (Segment->segmentName() != SegmentName) {
                            continue;
                        }
                    }

                    if (Segment->isProtected(IsBigEndian)) {
                        return Result.set(RunError::ProtectedSegment);
                    }

                    auto Section =
                        static_cast<
                            const MachO::SegmentCommand64::Section *>(nullptr);

                    const auto IterateResult =
                        IterateSections(*Segment,
                                        SectionName,
                                        IsBigEndian,
                                        Result,
                                        Section);

                    if (IterateResult != 0) {
                        return Result;
                    }

                    if (Section == nullptr) {
                        continue;
                    }

                    const auto SectionRange = Section->fileRange(IsBigEndian);

                    SectionFileOff = SectionRange.begin();
                    SectionAddr = Section->addr(IsBigEndian);
                    SectionSize = SectionRange.size();
                    SectionData =
                        MachO.map().getFromRange<const char>(SectionRange);

                    break;
                }
            }
        } else {
            for (const auto &LC : MachO.loadCommandsMap()) {
                using namespace MachO;
                if (const auto Segment =
                        dyn_cast<LoadCommandKind::Segment>(&LC, IsBigEndian))
                {
                    if (const auto SegName = SegmentName) {
                        if (Segment->segmentName() != SegmentName) {
                            continue;
                        }
                    }

                    if (Segment->isProtected(IsBigEndian)) {
                        return Result.set(RunError::ProtectedSegment);
                    }

                    auto Section =
                        static_cast<
                            const MachO::SegmentCommand::Section *>(nullptr);

                    const auto IterateResult =
                        IterateSections(*Segment,
                                        SectionName,
                                        IsBigEndian,
                                        Result,
                                        Section);

                    if (IterateResult != 0) {
                        return Result;
                    }

                    if (Section == nullptr) {
                        continue;
                    }

                    const auto SectionRange = Section->fileRange(IsBigEndian);

                    SectionFileOff = SectionRange.begin();
                    SectionAddr = Section->addr(IsBigEndian);
                    SectionSize = SectionRange.size();
                    SectionData =
                        MachO.map().getFromRange<const char>(SectionRange);

                    break;
                }
            }
        }

        if (SectionData == nullptr) {
            return Result.set(RunError::SectionNotFound);
        }

        auto LongestCStringLength = uint64_t();
        auto CStringInfoList =
            HandleCStringSection(SectionData,
                                 SectionFileOff,
                                 SectionAddr,
                                 SectionSize,
                                 Opt.Limit,
                                 LongestCStringLength);

        if (CStringInfoList.empty()) {
            return Result.set(RunError::HasNoStrings);
        }

        if (Opt.Sort) {
            std::sort(CStringInfoList.begin(),
                      CStringInfoList.end(),
                      [](const auto &Left, const auto &Right) noexcept {
                        return Left.String < Right.String;
                      });
        }

        const auto CStringListSizeDigitCount =
            Utils::GetIntegerDigitCount(CStringInfoList.size());

        auto Counter = static_cast<uint64_t>(1);
        for (const auto &Info : CStringInfoList) {
            fprintf(OutFile,
                    "C-String %0*" PRIu64 ": ",
                    CStringListSizeDigitCount,
                    Counter);

            Utils::PrintAddress(OutFile, Info.Address, Is64Bit);
            const auto WrittenOutString =
                fprintf(OutFile,
                        " \"" STRING_VIEW_FMT "\"",
                        STRING_VIEW_FMT_ARGS(Info.String));

            if (Opt.Verbose) {
                Utils::RightPadSpaces(
                    OutFile,
                    WrittenOutString,
                    LongestCStringLength + STR_LENGTH(" \"\""));

                fprintf(OutFile,
                        " (Length: %0*" PRIuPTR ", File Offset: ",
                        CStringListSizeDigitCount,
                        Info.String.length());

                Utils::PrintAddress(OutFile, Info.FileOffset, Is64Bit, "", ")");
            }

            fputc('\n', OutFile);
            Counter++;
        }

        return Result.set(RunError::None);
    }

    auto
    PrintCStringSection::run(const Objects::Base &Base) const noexcept
        -> RunResult
    {
        switch (Base.kind()) {
            case Objects::Kind::None:
                assert(false && "run() got Object with Kind::None");
            case Objects::Kind::MachO:
                return run(static_cast<const Objects::MachO &>(Base));
            case Objects::Kind::FatMachO:
                return RunResultUnsupported;
        }

        assert(false &&
               "Got unrecognized Object-Kind in PrintCStringSection::run");
    }
}
