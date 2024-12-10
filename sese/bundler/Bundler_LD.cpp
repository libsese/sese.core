#include "Bundler.h"
#include "sese/util/Exception.h"
#include <sese/system/Process.h>

#include <queue>

std::string Bundler::sanitizeFilename(const std::string &name) {
    std::string output = name;
    const std::string INVALID_CHARS = "-+*/@%&!?=^~:;,.";
    std::replace_if(output.begin(), output.end(), [&](char c) {
        return INVALID_CHARS.find(c) != std::string::npos || std::iscntrl(c);
    },
                    '_');
    return output;
}

void Bundler::write_ld_header_file() {
    auto result = sese::io::File::createEx(generate_code_path + "/" + class_name + ".h", "wt");
    if (result) {
        throw sese::Exception("Failed to open LD header file to write");
    }
    auto file = result.get();

    std::string header =
        "// Please do not modify this file manually\n"
        "// This file is generated by Bundler\n\n"
        "#pragma once\n\n";
    std::string str1 = "class ";
    std::string str2 =
        " {\n"
        "public: \n"
        "enum class Binaries {\n";
    std::string str3 =
        "};\n"
        "static char* syms[";
    std::string str4 =
        "];\n"
        "};";

    file->write(header.data(), header.length());
    file->write(str1.data(), str1.length());
    file->write(class_name.data(), class_name.length());
    file->write(str2.data(), str2.length());

    int index = 0;
    sese::text::StringBuilder builder(1024);
    for (auto &[k, _]: binaries) {
        builder << k << " = " << std::to_string(index) << ",\n";
        index += 1;
        auto str = builder.toString();
        builder.clear();
        file->write(str.data(), str.length());
    }

    file->write(str3.data(), str3.length());
    std::string count = std::to_string(binaries.size() * 2);
    file->write(count.data(), count.length());
    file->write(str4.data(), str4.length());
    file->close();
}

void Bundler::write_ld_source_file() {
    auto result = sese::io::File::createEx(generate_code_path + "/" + class_name + ".cpp", "wt");
    if (result) {
        throw sese::Exception("Failed to open LD source file to write");
    }
    auto file = result.get();

    std::queue<std::string> names;

    sese::text::StringBuilder builder(1024);
    builder << "#include \"" << class_name << ".h\"\n\n";
    auto header = builder.toString();
    builder.clear();
    file->write(header.data(), header.length());

    for (auto &[_, v]: binaries) {
        auto name = sanitizeFilename(v);
        names.emplace(name);
        builder << "extern char _binary_" << name << "_start[];\n";
        builder << "extern char _binary_" << name << "_end[];\n";
        auto str = builder.toString();
        builder.clear();
        file->write(str.data(), str.length());
    }

    builder << "\nchar * " << class_name << "::syms[" << std::to_string(binaries.size() * 2) << "] {\n";
    header = builder.toString();
    builder.clear();
    file->write(header.data(), header.length());
    while (!names.empty()) {
        builder << "\t_binary_" << names.front() << "_start,\n";
        builder << "\t_binary_" << names.front() << "_end,\n";
        names.pop();
        auto str = builder.toString();
        builder.clear();
        file->write(str.data(), str.length());
    }
    file->write("};\n", 3);
}

void Bundler::make_ld_resources() {
    // ld -r -b binary -o ${dest} ...${inputs}
    sese::text::StringBuilder builder(4096);
    builder << "ld -r -b binary";
    for (auto &[_, v]: binaries) {
        builder << " " << v;
    }
    builder << " -o " << base_path << "/" << generate_code_path << "/" << class_name << ".o";
    auto cmd = builder.toString();
    auto result = sese::system::Process::createEx(cmd.c_str());
    if (result) {
        throw sese::Exception("Failed to execute ld");
    }
    auto &p = result.get();
    if (p->wait()) {
        throw sese::Exception("Ld returned with non-zero value");
    }
}