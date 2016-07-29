/*
 * @file SelfRegistry.hpp
 *
 *  Created on: Jun 21, 2016
 *      @author: Gregory M. Baumgardner
 */

#ifndef SRC_SELFREGISTRY_HPP_
#define SRC_SELFREGISTRY_HPP_

#include <cstdio>
#include <vector>
#include <tmx/messages/message.hpp>

struct typeAllocator
{
public:
	typeAllocator(std::string name)
	{
		typeName = name;
		getAllocators().push_back(this);
	}

	virtual ~typeAllocator()
	{

	}

	static std::vector<typeAllocator *> &getAllocators()
	{
		static std::vector<typeAllocator *> allocs;
		return allocs;
	}

	virtual void print(FILE *, const char *) = 0;
	std::string typeName;
};

template <typename MsgType>
struct typeAllocatorImpl: public typeAllocator
{
public:
	typeAllocatorImpl(std::string name): typeAllocator(name) {}

	void print(FILE *file, const char *format)
	{
		std::string name = typeName;
		std::transform(name.begin(), name.end(), name.begin(), toupper);

		fprintf(file, format, name.c_str(), MsgType::MessageType, name.c_str(), MsgType::MessageSubType);
	}
};

#endif /* SRC_SELFREGISTRY_HPP_ */
