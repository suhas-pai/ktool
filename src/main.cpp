//
//  src/main.cpp
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#include <memory>
#include <string_view>
#include <vector>

#include "ADT/FileMap.h"
#include "MachO/LoadCommands.h"
#include "Operations/PrintHeader.h"

struct Operation {
    const char *Path;
    Operations::Kind Kind;
    std::unique_ptr<Operations::Base> Op;
};

auto main(const int argc, const char *const argv[]) -> int {
    auto OperationList = std::vector<Operation>();
    for (auto I = 1; I < argc; I++) {
        auto Arg = std::string_view(argv[I]);
        if (Arg.front() != '-') {
            fprintf(stderr,
                    "Expected an option, found \"%s\" instead\n",
                    Arg.data());
            return 1;
        }

        const auto HasNextArg = (I + 1) < argc;
        if (Arg == "-h" || Arg == "--header") {
            if (!HasNextArg) {
                fprintf(stderr,
                        "Option %s needs a path to a file/directory\n",
                        Arg.data());
                return 1;
            }

            auto Options = Operations::PrintHeader::Options();
            I++;

            if (strcmp(argv[I], "-v") == 0 ||
                strcmp(argv[I], "--verbose") == 0)
            {
                Options.Verbose = true;
                I++;
            }

            const auto Path = std::string_view(argv[I]);
            if (Path.front() == '-') {
                fprintf(stderr,
                        "Option %s needs a path to a file/directory, found %s "
                        "instead\n",
                        Arg.data(),
                        Path.data());
                return 1;
            }

            OperationList.push_back({
                Path.data(),
                Operations::Kind::PrintHeader,
                std::unique_ptr<Operations::PrintHeader>(
                    new Operations::PrintHeader(stdout, Options))
            });
        } else {
            fprintf(stderr, "Unrecognized option: %s\n", Arg.data());
            return 1;
        }
    }

    for (const auto &Operation : OperationList) {
        switch (Operation.Kind) {
            case Operations::Kind::PrintHeader: {
                const auto Map =
                    ADT::FileMap(Operation.Path, ADT::FileMap::Prot::Read);

                const auto Object = Objects::Open(Map.map());
                if (Object == nullptr) {
                    fprintf(stderr,
                            "File (at path %s) is not recognized\n",
                            Operation.Path);
                    return 1;
                }

                if (!Operation.Op->run(*Object)) {
                    fprintf(stderr,
                            "print-header isn't supported for file at "
                            "path %s\n",
                            Operation.Path);
                    return 1;
                }
            }
        }
    }

    return 0;
}
