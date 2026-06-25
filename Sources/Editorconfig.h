/*
 * Copyright 2018 Kacper Kasper <kacperkasper@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H


#include <Message.h>
#include <Path.h>


class Editorconfig {
public:
	static bool Find(BPath* filePath, BPath* editorconfigPath);
	static bool Parse(const char* filename, BMessage* propertiesDict);
	static void MatchFilename(const char* filename,
			const BMessage* allProperties, BMessage* properties);
};


#endif // EDITORCONFIG_H
