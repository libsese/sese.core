#include "Bundler.h"

void Bundler::write_rc_file() {
    auto result = sese::io::File::createEx(generate_code_path + "/" + class_name + ".rc", "wt");
    if (result) {
        throw sese::Exception("Failed to open RC file to write");
    }
    auto file = result.get();
    sese::text::StringBuilder builder(1024);
    builder << "#include \"" << class_name << ".h\"\n\n";
    auto include = builder.toString();
    builder.clear();
    file->write(include.data(), include.length());

    for (auto &[k, v]: binaries) {
        builder << "BIN_" << k << " ";
        builder << "RCDATA ";
        builder << "\"" << v << "\"\n";
        auto string = builder.toString();
        builder.clear();
        file->write(string.data(), string.length());
    }
    file->close();
}

void Bundler::write_rc_resource_file() {
    auto result = sese::io::File::createEx(generate_code_path + "/" + class_name + ".h", "wt");
    if (result) {
        throw sese::Exception("Failed to open `Resource` file to write");
    }
    auto file = result.get();

    std::string header =
        "// Please do not modify this file manually\n"
        "// This file is generated by Bundler\n\n"
        "#pragma once\n\n";

    std::string str1 =
        "class ";
    std::string str2 =
        " {\n"
        "public: \n";
    std::string str3 = "enum class Binaries : int {\n";

    file->write(header.data(), header.length());
    sese::text::StringBuilder builder(1024);
    {
        file->write("#ifdef RC_INVOKED\n", 18);
        int index = 1000;
        for (auto &[k, _]: binaries) {
            index += 1;
            builder << "#define BIN_" << k << " " << std::to_string(index) << "\n";
            auto string = builder.toString();
            builder.clear();
            file->write(string.data(), string.length());
        }
        file->write("#endif\n\n", 8);
    }
    {
        file->write(str1.data(), str1.length());
        file->write(class_name.data(), class_name.length());
        file->write(str2.data(), str2.length());
        int index = 1000;
        file->write(str3.data(), str3.length());
        bool first = true;
        for (auto &[k, _]: binaries) {
            index += 1;
            if (!first) {
                builder << ", ";
            }
            first = false;
            builder << "\t" << k << " = " << std::to_string(index) << "\n";
            auto string = builder.toString();
            builder.clear();
            file->write(string.data(), string.length());
        }
        file->write("};\n", 3);
    }
    file->write("};\n", 3);
}