/*
 * RtcmMessageFactory.h
 *
 * A header for the RTCM message factory
 *
 *  Created on: May 31, 2018
 *      @author: Gregory M. Baumgardner
 */

#ifndef INCLUDE_RTCM_RTCMMESSAGEFACTORY_H_
#define INCLUDE_RTCM_RTCMMESSAGEFACTORY_H_

#include "RtcmVersion.h"
#include "RtcmMessage.h"

#include <array>
#include <vector>

namespace tmx {
namespace messages {
namespace rtcm {


class RtcmMessageFactory {
public:
	static TmxRtcmMessage *create(const tmx::byte_stream &bytes, RTCM_VERSION version = UNKNOWN, msgtype_type type = 0) {
		auto factory = get_Instance();
		TmxRtcmMessage *ptr = NULL;

		if (type < factory._types[version].size() &&
			factory._types[version][type])
			ptr = factory._types[version][type]->allocate();
		else
			ptr = factory._types[version][0]->allocate();

		if (ptr)
			ptr->set_contents(bytes);
		return ptr;
	}

	static TmxRtcmMessage *create(const tmx::byte_stream &bytes, std::string version, msgtype_type type = 0) {
		return create(bytes, RtcmVersion(version), type);
	}

	static TmxRtcmMessage *create(TmxRtcmMessage *other) {
		if (!other) return NULL;
		return create(other->get_contents(), other->get_Version(), other->get_MessageType());
	}

private:

	/**
	 * Base template for allocator
	 */
	struct msgtype_allocator {
		virtual TmxRtcmMessage *allocate() { return NULL; }
	};

	template <class MsgType>
	struct msgtype_allocatorImpl: public msgtype_allocator {
		TmxRtcmMessage *allocate() { return new MsgType(); }
	};

	std::array<std::vector<msgtype_allocator *>, RTCM_VERSION::RTCM_EOF> _types;

	/**
	 * Base template for the registrar structure
	 */
	template<class... T> struct msgtype_registrar;

	/**
	 * Template specialization for an RTCM message type
	 */
	template<template<RTCM_VERSION, msgtype_type> class MsgType, RTCM_VERSION V, msgtype_type T>
	struct msgtype_registrar<MsgType<V, T>> {
		static void registerType(decltype(_types) &types) {
			if (types[V].size() <= T) types[V].resize(T+1, NULL);
			types[V][T] = new msgtype_allocatorImpl<MsgType<V, T>>();
		}
	};

	/**
	 * Variadic template specialization
	 */
	template<class MsgType, class... Others>
	struct msgtype_registrar<MsgType, Others...> {
		static void registerType(decltype(_types) &types) {
			msgtype_registrar<MsgType>::registerType(types);
			msgtype_registrar<Others...>::registerType(types);
		}
	};

	static RtcmMessageFactory &get_Instance() {
		static RtcmMessageFactory factory;

		// Initialize the types, if not done already
		if (!factory._types[rtcm::UNKNOWN].size()) {
			factory.registerTypes<RTCM_EOF>();
		}

		return factory;
	}

	template <template <class...> class T, class... Types>
	void registrarDispatch(T<Types...> &tuple) {
		msgtype_registrar<Types...>::registerType(_types);
	}

	template <RTCM_VERSION Version>
	void registrarDispatch() {
		static typename RtcmMessageTypeBox<Version>::types tuple;
		registrarDispatch(tuple);
	}

	template <RTCM_VERSION Version>
	void registerTypes() {
		if (Version < RTCM_EOF) {
			registrarDispatch<Version>();
		}

		registerTypes<(RTCM_VERSION)((int)Version - 1)>();
	}

};

template <>
void RtcmMessageFactory::registerTypes<UNKNOWN>() {
	registrarDispatch<UNKNOWN>();
}


} /* End namespace rtcm */
} /* End namespace messages */
} /* End namspace tmx */


#endif /* INCLUDE_RTCM_RTCMMESSAGEFACTORY_H_ */
