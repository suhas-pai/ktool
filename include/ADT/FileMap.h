//
//  ADT/FileMap.h
//  ktool
//
//  Created by suhaspai on 11/13/22.
//

#pragma once

#include "FlagsBase.h"
#include "MemoryMap.h"
#include "PointerOrError.h"
#include "Range.h"

namespace ADT {
    struct FileMap {
    protected:
        void *Base = nullptr;
        uint64_t Size = 0;

        explicit FileMap(void *const Base, const uint64_t Size) noexcept
        : Base(Base), Size(Size) {}
    public:
        enum class Prot {
            Read = 0b1,
            Write = 0b10,
            Execute = 0b100,

            ReadWrite = 0b11,
            ReadExecute = 0b101,

            WriteExecute = 0b110,
            ReadWriteExecute = 0b111
        };

        enum class MapKind : uint32_t {
            Private,
            Shared
        };

        enum class OpenError {
            None,
            FailedToOpen,
            FailedToStat,
            FailedToMemMap,
        };

        static auto
        Open(const char *Path, Prot Prot) noexcept
            -> PointerOrError<FileMap, OpenError>;

        explicit FileMap(const FileMap &FileMap) noexcept = delete;
        FileMap(FileMap &&FileMap) noexcept = default;

        ~FileMap() noexcept;

        [[nodiscard]] constexpr auto size() const noexcept { return Size; }

        template <typename T = void, bool Verify = true>
        [[nodiscard]] constexpr auto base() const noexcept {
            if constexpr (Verify) {
                if (size() < sizeof(T)) {
                    return nullptr;
                }
            }

            return static_cast<T *>(Base);
        }

        [[nodiscard]] constexpr auto range() const noexcept {
            return Range::FromSize(0, Size);
        }

        [[nodiscard]] constexpr auto map() const noexcept {
            return MemoryMap(Base, Size);
        }
    };

    MAKE_ENUM_MASK_CLASS(FileMap::Prot)
}
