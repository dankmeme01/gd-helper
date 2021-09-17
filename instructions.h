#pragma once
#define GAME_NAME "GeometryDash.exe"
#include <cstring>
#include <map>
#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <iostream>
#include "presets.h"
#include "download.h"
void tokenize(std::string const&, const char*, std::vector<std::string>&);
HANDLE GetProcessByName(PCSTR);

/*
DOCS FOR COMMANDS:
[] means optional argument
$ means command argument

download: (string/var url, string/var filename, [string/var location] = Temp) - downloads a file from $url to $location/$filename
set: (string var, string/var value) - sets a variable with name $var to $value
findgd: () - sets $gmd to geometry dash folder or throws if gd is not started
findsong: () - sets $gmdsong to appdata/local/geometrydash
deletesong: (string/var id) - deletes $id.mp3 from $gmdsong, throws if undefined
stdout: (string/var data) - outputs $data to STDOUT
stderr: (string/var data) - outputs $data to STDERR
input: (string varname) - sets $varname variable to data inputted by user
*/

class Instruction {
	std::vector<std::string> instructions;
    std::map<std::string, std::string> vars;
public:
	Instruction(std::string instruction) {
        tokenize(instruction, ";", this->instructions);
	}
    Instruction(int magicNumber) {
        tokenize(Presets::get(magicNumber), ";", this->instructions);
    }
    void runCommand(std::string command, std::vector<std::string> args, int instruction) {
        if (command == "download") {
            if (args.size() < 2) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to download command, should be (URL, name, [saveloc])");
            else if (args.size() == 2) { DownloadFile(args.at(0), args.at(1)); }
            else if (args.size() == 3) { DownloadFile(args.at(0), args.at(1), args.at(2)); }
            else throw InvalidInstructionError(instruction, "Argument overload! Expected 2-3 arguments, got " + std::to_string(args.size()));
        }
        else if (command == "set") {
            if (args.size() < 2) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to set command, should be (name, value)");
            else if (args.size() == 2) {
                this->setVar(args.at(0), args.at(1));
            }
            else throw InvalidInstructionError(instruction, "Argument overload! Expected 2-3 arguments, got " + std::to_string(args.size()));
        }
        else if (command == "findgd") {
            fs::path gdpath;
            HANDLE gmd = GetProcessByName(GAME_NAME);
            if (gmd != INVALID_HANDLE_VALUE) {
                TCHAR filename[MAX_PATH];
                if (GetModuleFileNameEx(gmd, NULL, filename, MAX_PATH) == 0) throw GameNotExists();
                else {
                    gdpath = fs::path(filename).parent_path();
                }
            }
            this->setVar("gmd", gdpath.string());
        }
        else if (command == "findsong") {
            fs::path gdpath;
            gdpath = std::filesystem::temp_directory_path().parent_path().parent_path() / "GeometryDash";
            if (!std::filesystem::exists(gdpath)) std::filesystem::create_directories(gdpath);
            this->setVar("gmdsong", gdpath.string());
        }
        else if (command == "deletesong") {
            auto gmd = this->getVar("gmdsong");
            if (args.size() < 1) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to deletesong command, should be (song ID)");
            else if (args.size() == 1) {
                auto fullname = fs::path(gmd) / (args.at(0) + ".mp3");
                if (fs::exists(fullname)) {
                    fs::remove(fullname);
                }
            }
            else throw InvalidInstructionError(instruction, "Argument overload! Expected 1 argument, got " + std::to_string(args.size()));
            if (gmd == "") throw InvalidInstructionError(instruction, "Trying to run deletesong command before running findsong");
        }
        else if (command == "stdout") {
            if (args.size() < 1) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to stdout command, should be (var/text)");
            else if (args.size() > 1) throw InvalidInstructionError(instruction, "Argument overload! Expected 1 argument, got " + std::to_string(args.size()));
            std::cout << args.at(0);
        }
        else if (command == "stderr") {
            if (args.size() < 1) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to stderr command, should be (var/text)");
            else if (args.size() > 1) throw InvalidInstructionError(instruction, "Argument overload! Expected 1 argument, got " + std::to_string(args.size()));
            std::cerr << args.at(0);
        }
        else if (command == "input") {
            if (args.size() < 1) throw InvalidInstructionError(instruction, "Invalid amount of arguments given to input command, should be (varName)");
            else if (args.size() > 1) throw InvalidInstructionError(instruction, "Argument overload! Expected 1 argument, got " + std::to_string(args.size()));
            std::string _inp;
            getline(std::cin, _inp);
            this->setVar(args.at(1), _inp);
        }
        else throw InvalidInstructionError(instruction, "Invalid command: " + command);
    }
    void execute() {
        int num = 0;
        for (const std::string& in : this->instructions) {
            if (in.empty()) continue;
            std::vector<std::string> args;
            tokenize(in, ",", args);
            std::string cmd = args.at(0);
            args.erase(args.begin());
            for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it) {
                auto ind = it - args.begin();
                std::string m = *it;
                if (m.find("$") == 0) {
                    m.erase(0, 1);
                    if(this->getVar(m) == "") throw InvalidInstructionError(num + 1, "Trying to access non-existent variable " + m);
                    args.at(ind) = this->vars.at(m);
                }
            }
            this->runCommand(cmd, args, ++num);
        }
    }

    void setVar(std::string name, std::string value) {
        if (this->vars.count(name) == 1) this->vars.erase(name);
        this->vars.insert(std::pair<std::string, std::string>(name, value));
    }

    std::string getVar(std::string key) {
        if (this->vars.count(key) == 0) return "";
        return this->vars.at(key);
    }
};

void tokenize(std::string const& str, const char* delim, std::vector<std::string>& out)
{
    char* next = NULL;
    char* token = strtok_s(const_cast<char*>(str.c_str()), delim, &next);
    while (token != NULL)
    {
        out.push_back(std::string(token));
        token = strtok_s(NULL, delim, &next);
    }
}

HANDLE GetProcessByName(PCSTR name) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
        do {
            auto exe = process.szExeFile;
            std::wstring exeFixed2(exe);
            std::string name_(name);
            if (exeFixed2 == std::wstring(name_.begin(), name_.end())) {
                pid = process.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);
    if (pid != 0) { return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid); }
    return NULL;
}