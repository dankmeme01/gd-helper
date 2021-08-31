#pragma once
#include "errors.h"
#include <vector>

std::vector<std::string> _presets = {
	"download,https://github.com/yourkin/fileupload-fastapi/raw/a85a697cab2f887780b3278059a0dd52847d80f3/tests/data/test-10mb.bin,10mbtrash.bin",
	"set,name,anothername.bin;download,https://github.com/yourkin/fileupload-fastapi/raw/a85a697cab2f887780b3278059a0dd52847d80f3/tests/data/test-10mb.bin,$name",
	"set,id,436814;set,name,436814.mp3;findsong;set,link,https://music-music-mp3.com/api/YzY2ZGMzY2VjNmUzZTg0ZTI5YjNiN2VhOGE1ODE3YWZ8bG9hZA.mp3;download,$link,$name,$gmdsong"
};

class Presets {
public:
	static std::string get(size_t index) {
		if (index >= _presets.size() || index < (size_t)0) throw IndexOutOfBoundsError(index, _presets.size());
		return _presets.at(index);
	}
};