/*
 * Copyright 2018 Kacper Kasper <kacperkasper@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Editorconfig.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

#include <Directory.h>
#include <Entry.h>
#include <String.h>


bool
Editorconfig::Find(BPath* filePath, BPath* editorconfigPath)
{
	if(filePath == nullptr) return false;

	bool found = false;
	BPath parentPath(*filePath);
	BDirectory parentDir;
	BEntry editorconfigFile;
	do {
		parentPath.GetParent(&parentPath);
		parentDir.SetTo(parentPath.Path());
		editorconfigFile.SetTo(&parentDir, ".editorconfig");
		found = editorconfigFile.Exists();
	} while(!found && !parentDir.IsRootDirectory());
	if(found && editorconfigPath != nullptr) {
		editorconfigFile.GetPath(editorconfigPath);
	}
	return found;
}


bool
Editorconfig::Parse(const char* filename, BMessage* propertiesDict)
{
	if(propertiesDict == nullptr) return false;

	std::ifstream file(filename, std::ifstream::in);
	if(!file.is_open()) return false;

	std::string currentSectionName;
	BMessage currentSection;

	std::string line;
	do {
		std::getline(file, line);
		if(line.empty() || line[0] == '#' || line[0] == ';') continue;
		if(line[0] == '[' && line[line.size() - 1] == ']') {
			// section
			if(!currentSectionName.empty() && !currentSection.IsEmpty()) {
				propertiesDict->AddMessage(currentSectionName.c_str(), &currentSection);
				currentSection.MakeEmpty();
			}
			currentSectionName = line.substr(1, line.size() - 2);
		} else {
			// property
			std::string name, value;
			line.erase(std::remove_if(line.begin(), line.end(), [](char x) { return std::isspace(x); }), line.end());
			std::istringstream linestream(line);
			std::getline(linestream, name, '=');
			std::getline(linestream, value, '=');
			currentSection.AddString(name.c_str(), value.c_str());
		}
	} while(!file.eof());
	if(!currentSectionName.empty() && !currentSection.IsEmpty()) {
		propertiesDict->AddMessage(currentSectionName.c_str(), &currentSection);
	}
	file.close();
	return true;
}


void
Editorconfig::MatchFilename(const char* filename, const BMessage* allProperties,
	BMessage* properties)
{
	if(allProperties == nullptr) return;

	char* name;
	uint32 type;
	int32 count;
	for(int32 i = 0;
			allProperties->GetInfo(B_MESSAGE_TYPE, i, &name, &type, &count) == B_OK;
			i++) {
		BString regexStr(name);
		bool dirSpecific = false;
		int32 slashIndex = regexStr.FindFirst("/");
		if(slashIndex != B_ERROR && slashIndex > 0 && regexStr[slashIndex - 1] != '\\')
			dirSpecific = true;
		int32 inBraceCount = 0;
		int32 c = 0;
		while(c < regexStr.Length()) {
			if(regexStr[c] == '*') {
				if(c > 0 && regexStr[c - 1] == '\\') {
					c++;
					continue;
				}
				// **
				if(c < regexStr.Length() - 1 && regexStr[c + 1] == '*') {
					regexStr.Replace("**", "(.+)", 1, c);
					c += strlen("(.+)");
				} else {
					BString s;
					if(dirSpecific) s = "([^/]+)";
					else s = "(.+)";
					regexStr.Replace("*", s, 1, c);
					c += s.Length();
				}
				continue;
			} else if(regexStr[c] == '!') {
				// FIXME: only if in []?
				if(c > 0 && regexStr[c - 1] == '\\') {
					c++;
					continue;
				}
				regexStr.Replace("!", "^", 1, c);
			} else if(regexStr[c] == '.') {
				regexStr.Replace(".", "\\.", 1, c);
				c += 2;
				continue;
			}
			if(regexStr[c] == '{') {
				if(c > 0 && regexStr[c - 1] == '\\') {
					c++;
					continue;
				}
				inBraceCount++;
				regexStr.Replace("{", "(", 1, c);
			}
			if(inBraceCount > 0) {
				if(c > 0 && regexStr[c - 1] == '\\') {
					c++;
					continue;
				}
				if(regexStr[c] == ',')
					regexStr.Replace(",", "|", 1, c);
				if(regexStr[c] == '}') {
					inBraceCount--;
					regexStr.Replace("}", ")", 1, c);
				}
			}
			c++;
		}

		if(inBraceCount != 0) return;

		std::regex expr(regexStr.String());
		if(properties != nullptr && std::regex_match(filename, expr)) {
			BMessage globProperties;
			if(allProperties->FindMessage(name, &globProperties) == B_OK) {
				char* stringName;
				for(int32 j = 0;
						globProperties.GetInfo(B_STRING_TYPE, j, &stringName, &type, &count) == B_OK;
						j++) {
					BString value;
					globProperties.FindString(stringName, &value);
					properties->RemoveName(stringName);
					properties->AddString(stringName, value);
				}
			}
		}
	}
}
