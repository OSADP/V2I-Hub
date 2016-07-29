/*
 * @file Main.cpp
 *
 *  Created on: Jun 21, 2016
 *      @author: Gregory M. Baumgardner
 */

#include <algorithm>
#include <iostream>
#include "SelfRegistry.hpp"

int compareAllocs(typeAllocator *a, typeAllocator *b)
{
	if (a == NULL || b == NULL)
		return 1;

	std::cout << "Comparing " << a->typeName << " to " << b->typeName << std::endl;
	return (a->typeName.compare(b->typeName));
}

int main(int argc, char *argv[])
{
	const char *jsFormat = "const %s_TYPE = \"%s\";\nconst %s_SUBTYPE = \"%s\";\n";
	const char *csFormat = "public const string %s_TYPE = \"%s\";\npublic const string %s_SUBTYPE = \"%s\";\n";

	std::string className = "MessageTypes";

	if (argc > 1)
	{
		className = argv[1];
	}

	std::string jsFileName = className;
	std::string csFileName = jsFileName;
	jsFileName += ".js";
	csFileName += ".cs";

	FILE *jsFile = fopen(jsFileName.c_str(), "w");
	FILE *csFile = fopen(csFileName.c_str(), "w");

	fprintf(csFile, "namespace CVIS.Core.Messages {\n");
	fprintf(csFile, "public class %s {\n", className.c_str());

	std::vector<typeAllocator *> all = typeAllocator::getAllocators();
	//std::sort(all.begin(), all.end(), compareAllocs);

	std::cout << "Found " << all.size() << " message types." << std::endl;

	for (std::vector<typeAllocator *>::iterator i = all.begin(); i != all.end(); i++)
	{
		(*i)->print(jsFile, jsFormat);
		(*i)->print(csFile, csFormat);
	}

	fprintf(csFile, "}\n}\n");
	fclose(jsFile);
	fclose(csFile);
}


