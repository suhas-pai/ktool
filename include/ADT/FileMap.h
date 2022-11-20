//
//  ADT/FileMap.h
//  ktool
//
//  Created by suhaspai on 11/13/22.
//

#include <cstdint>

#include "FlagsBase.h"
#include "MemoryMap.h"
#include "Range.h"

namespace ADT {
    struct FileMap {
    protected:
        void *Base = nullptr;
        uint64_t Size = 0;
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

        explicit FileMap(const char *Path, Prot Prot) noexcept;
        explicit FileMap(const FileMap &FileMap) noexcept = delete;

        ~FileMap() noexcept;

        [[nodiscard]] constexpr auto size() const noexcept { return Size; }

        template <typename T = void>
        [[nodiscard]] constexpr auto base() const noexcept {
            if (size() < sizeof(T)) {
                return nullptr;
            }

            return static_cast<T *>(Base);
        }

        [[nodiscard]] inline auto range() const noexcept {
            return Range::FromSize(reinterpret_cast<uint64_t>(Base), Size);
        }

        [[nodiscard]] constexpr auto map() const noexcept {
            return MemoryMap(Base, Size);
        }
    };

    MAKE_ENUM_MASK_CLASS(FileMap::Prot)
}
