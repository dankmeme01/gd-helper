#pragma once
#include <exception>
#include <stdexcept>
#include <string>

class FileNotFoundError : public std::runtime_error {
public:
	FileNotFoundError(std::string reqPath):runtime_error("File was not found by path " + reqPath) {}
};

class DownloadError : public std::runtime_error {
public:
	DownloadError(std::string errorString) :runtime_error("Download error: " + errorString) {}
};

class InvalidInstructionError : public std::runtime_error {
public:
	InvalidInstructionError(int instNum, std::string error) :runtime_error("Error in executing instruction number " + std::to_string(instNum) + ": " + error) {}
	InvalidInstructionError(std::string msg) :runtime_error(msg.c_str()) {};
};

class IndexOutOfBoundsError : public std::runtime_error {
public:
	IndexOutOfBoundsError(size_t given, size_t max) :runtime_error("Invalid index was provided: " + std::to_string(given) + " when list size is " + std::to_string(max)) {}
	IndexOutOfBoundsError(std::string msg) :runtime_error(msg.c_str()) {};
};

class GameNotExists : public std::runtime_error {
public:
	GameNotExists() :runtime_error("Geometry Dash is not started up, please fix") {}
	GameNotExists(std::string msg) :runtime_error(msg.c_str()) {};
};