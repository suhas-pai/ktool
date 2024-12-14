//
//  ADT/OutputFile.cpp
//  ktool
//
//  Created by Suhas Pai on 4/29/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cassert>
#include <string>

#include "ADT/OutputFile.h"

OutputFile::OutputFile() noexcept {
    const auto RandNumber = rand() % 99999;
    const auto FileName = "Tmp." + std::to_string(RandNumber);

    this->Tmp = FileDescriptor::Create(FileName.data(), 0600);
}

bool OutputFile::Read(void *const Buf, const size_t Size) const noexcept {
    assert(this->isOpen() && "OutputFile must be open");
    return Tmp.Read(Buf, Size);
}

bool OutputFile::Write(const void *const Buf, const size_t Size) noexcept {
    assert(this->isOpen() && "OutputFile must be open");
    return Tmp.Write(Buf, Size);
}

bool OutputFile::TruncateToSize(const uint64_t Length) noexcept {
    assert(this->isOpen() && "OutputFile must be open");
    return Tmp.TruncateToSize(Length);
}

bool OutputFile::Finalize(const char *const Path) noexcept {
    Close();
    return (rename(this->TmpPath, Path) == 0);
}
