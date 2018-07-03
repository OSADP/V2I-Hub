/*
 * RTCM2.h
 *
 * Message definitions for RTCM 2.3
 *
 *  Created on: Apr 26, 2018
 *      @author: gmb
 */

#ifndef INCLUDE_RTCM_RTCM2_H_
#define INCLUDE_RTCM_RTCM2_H_

#include "RtcmMessage.h"
#include "RtcmDataManager.h"

namespace tmx {
namespace messages {
namespace rtcm {

enum RTCM2_MESSAGE_TYPE {
	DifferentialGPSCorrections = 1,
	DeltaDifferentialGPSCorrections = 2,
	GPSReferenceStationParameters = 3,
	ReferenceStationDatum = 4,
	GPSConstellationHealth = 5,
	GPSNullFrame = 6,
	DGPSRadiobeaconAlmanac = 7,
	PseudoliteAlmanac = 8,
	GPSPartialCorrectionSet = 9,
	P_CodeDifferentialCorrections = 10,
	CA_CodeL1L2DeltaCorrections = 11,
	PseudoliteStationParameters = 12,
	GroundTransmitterParameters = 13,
	GPSTimeOfWeek = 14,
	IonosphericDelayMessage = 15,
	GPSSpecialMessage = 16,
	GPSEphemerides = 17,
	RTKUncorrectedCarrierPhases = 18,
	RTKUncorrectedPseudoranges = 19,
	RTKCarrierPhaseCorrections = 20,
	RTKHi_AccuracyPseudorangeCorrections = 21,
	ExtendedReferenceStationParameters = 22,
	AntennaTypeDefinitionRecord = 23,
	AntennaReferentPoint = 24,
	ExtendedRadiobeaconAlmanac = 27,
	DifferentialGLONASSCorrections = 31,
	DifferentialGLONASSReferenceStationParameters = 32,
	GLONASSConstellationHealth = 33,
	GLONASSPartialDifferentialCorrectionSet = 34,
	GLONASSRadiobeaconAlmanac = 35,
	GLONASSSpecialMessage = 36,
	GNSSSystemTimeOffset = 37,
	RTCM2_MESSAGE_TYPE_EOF
};

} /* End namespace rtcm */

/**
 * Generic RTCM 2 message with header contents
 */
template <>
class RTCMMessage<rtcm::SC10402_3>: public TmxRtcmMessage {
public:
	enum StationHealthValue {
		UDREScaleFactor1_0 = 0,
		UDREScaleFactor0_75,
		UDREScaleFactor0_5S,
		UDREScaleFactor0_3,
		UDREScaleFactor0_2,
		UDREScaleFactor0_1,
		NotMontitored,
		NotWorking
	};


	RTCMMessage<rtcm::SC10402_3>(): TmxRtcmMessage() { this->get_Preamble(); }
	RTCMMessage<rtcm::SC10402_3>(const TmxRtcmMessage &other): TmxRtcmMessage(other) { this->get_Preamble(); }
	virtual ~RTCMMessage() { }

	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint8,  Preamble, 0x66, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint6,  FrameID, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint10, StationID, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint6,  Parity1, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint13, ModifiedZCount, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint3,  SequenceNumber, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint5,  NumberDataWords, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint3,  StationHealth, 0, );
	std_rtcm_attribute(this->msg, tmx::messages::rtcm::uint6,  Parity2, 0, );

protected:
	typedef rtcm::RtcmDataManager<rtcm::uint30, rtcm::uint6> datamgr_type;
	typedef typename datamgr_type::rtcm_word rtcm_word;
	typedef typename datamgr_type::data_type data_type;

	datamgr_type mgr { this->msg, 0x40 };
public:
	virtual rtcm::RTCM_VERSION get_Version() { return rtcm::SC10402_3; }
	virtual rtcm::msgtype_type get_MessageType() { return this->get_FrameID(); }
	void clear() { TmxRtcmMessage::clear(); mgr.clearData(); }

	virtual void set_contents(const tmx::byte_stream &in) {
		TmxRtcmMessage::set_contents(in);

		tmx::byte_stream &bytes = getBytes();

		try {
			size_t consumed = mgr.assignData(getBytes(),
					_Preamble,
					_FrameID,
					_StationID,
					_Parity1,
					_ModifiedZCount,
					_SequenceNumber,
					_NumberDataWords,
					_StationHealth,
					_Parity2);

			bytes.erase(bytes.begin(), bytes.begin() + consumed);
			mgr.loadData(bytes, get_NumberDataWords());

		} catch (TmxException &ex) {
			this->invalidate();
			return;
		}
	}

	virtual tmx::byte_stream get_contents() {
		return mgr.extractAllData(get_NumberDataWords(),
				_Preamble,
				_FrameID,
				_StationID,
				_Parity1,
				_ModifiedZCount,
				_SequenceNumber,
				_NumberDataWords,
				_StationHealth,
				_Parity2);
	}

	data_type get_Header1() {
		tmx::byte_stream h1 = mgr.extractData(_Preamble, _FrameID, _StationID, _Parity1);
		return mgr.readWord(h1);
	}

	data_type get_Header2() {
		tmx::byte_stream h2 = mgr.extractData(_ModifiedZCount, _SequenceNumber, _NumberDataWords, _StationHealth, _Parity2);
		return mgr.readWord(h2);
	}

	virtual bool is_Valid() {
		return TmxRtcmMessage::is_Valid() &&
				this->get_Preamble() == Preamble::default_value() &&
				this->check_Parity(this->get_Header1(), 0) &&
				this->check_Parity(this->get_Header2(), this->get_Parity1());
	}

	data_type add_Parity(data_type word, data_type prevWord) {
		typename rtcm_word::bitset w(word);

		tmx::byte_t D29 = rtcm::get_Bit<rtcm_word, 29>(prevWord);
		tmx::byte_t D30 = rtcm::get_Bit<rtcm_word, 30>(prevWord);

		w[5] = D29 ^
				rtcm::bit_manipulator<1, 2, 3, 5, 6, 10, 11, 12, 13, 14, 17, 18, 20, 23>::xor_bits<rtcm_word>(word);
		w[4] = D30 ^
				rtcm::bit_manipulator<2, 3, 4, 6, 7, 11, 12, 13, 14, 15, 18, 19, 21, 24>::xor_bits<rtcm_word>(word);
		w[3] = D29 ^
				rtcm::bit_manipulator<1, 3, 4, 5, 7, 8, 12, 13, 14, 15, 16, 19, 20, 22>::xor_bits<rtcm_word>(word);
		w[2] = D30 ^
				rtcm::bit_manipulator<2, 4, 5, 6, 8, 9, 13, 14, 15, 16, 17 ,20, 21, 23>::xor_bits<rtcm_word>(word);
		w[1] = D30 ^
				rtcm::bit_manipulator<1, 3, 5, 6, 7, 9, 10, 14, 15, 16, 17, 18, 21, 22, 24>::xor_bits<rtcm_word>(word);
		w[0] = D29 ^
				rtcm::bit_manipulator<3, 5, 6, 8, 9, 10, 11, 13, 15, 19, 22, 23, 24>::xor_bits<rtcm_word>(word);

		return (data_type)w.to_ulong();
	}

	void Validate() {
		this->set_Parity1(add_Parity(get_Header1(), 0) & Parity1::bitmask());
		this->set_Parity2(add_Parity(get_Header2(), this->get_Parity1()) & Parity2::bitmask());
	}

	virtual TmxRtcmMessage *clone() { return new RTCMMessage<rtcm::SC10402_3>(); }

protected:
	bool check_Parity(data_type word, data_type prevWord) {
		return (word == add_Parity(word, prevWord));
	}

};

typedef RTCMMessage<rtcm::RTCM_VERSION::SC10402_3> RTCM2Message;
/*
 * Generic empty message type for RTCM 2.3.
 */
template<>
class RTCMMessageType<rtcm::SC10402_3, 0>: public RTCMMessage<rtcm::SC10402_3> {
public:
	RTCMMessageType<rtcm::SC10402_3, 0>():
		RTCMMessage<rtcm::SC10402_3>() { }
	RTCMMessageType<rtcm::SC10402_3, 0>(const TmxRtcmMessage &other):
		RTCMMessage<rtcm::SC10402_3>(other) { }
};

/*
 * Some pre-defined message types for RTCM 2.3 that may be useful
 */
template <>
class RTCMMessageType<rtcm::SC10402_3, rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage>:
	public RTCM2Message {
	using super = RTCM2Message;
public:
	RTCMMessageType<rtcm::SC10402_3, rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage>():
		super() { this->set_FrameID(rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage); }
	RTCMMessageType<rtcm::SC10402_3, rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage>(const TmxRtcmMessage &other):
		super(other) { this->set_FrameID(rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage); }

	std_attribute(this->msg, std::string, SpecialMessage, "",
			this->set_NumberDataWords(value.length() / 3 + (value.length() % 3 == 0 ? 0 : 1)); );

	virtual void set_contents(const tmx::byte_stream &in) {
		typedef rtcm::uint8 charByte;

		super::set_contents(in);

		// Consume the words in the data buffer
		data_type prevWord = this->get_Parity2();
		std::string theMessage;
		for (size_t i = 0; i < mgr.numDataWords(); i++) {
			data_type word = mgr.getData(i);
			if (!this->check_Parity(word, prevWord)) {
				this->invalidate();
				return;
			}

			prevWord = word;

			word >>= Parity1::size;
			for (size_t n = 0; n < 3; n++) {
				char c = (char)((word >> (charByte::size * (2 - n))) & charByte::bitmask());
				if (n > 0 && c == 0x00) continue;
				theMessage.push_back(c);
			}
		}

		this->set_SpecialMessage(theMessage);
	}

	virtual tmx::byte_stream get_contents() {
		// Rebuild the data buffer
		std::string message = this->get_SpecialMessage();

		data_type prevWord = this->get_Parity2();

		for (size_t i = 0; i < message.length(); i++) {
			data_type word = (message[i++] << 22);
			word |= (i < message.length() ? (message[i++] << 14) : 0);
			word |= (i < message.length() ? (message[i]   <<  6) : 0);

			data_type w = this->add_Parity(word, prevWord);

			prevWord = w;
			mgr.loadWord(w);
		}

		return super::get_contents();
	}
};

typedef RTCMMessageType<rtcm::SC10402_3, 0> RTCM2UnknownMessage;
typedef RTCMMessageType<rtcm::SC10402_3, rtcm::RTCM2_MESSAGE_TYPE::GPSSpecialMessage> RTCM2GPSSpecialMessage;

namespace rtcm {

/**
 * Available RTCM 2 message types
 */
template <>
struct RtcmMessageTypeBox<SC10402_3> {
	typedef std::tuple<
				tmx::messages::RTCM2UnknownMessage,
				tmx::messages::RTCM2GPSSpecialMessage
			> types;
};

} /* End namespace rtcm */
} /* End namespace messages */
} /* End namespace tmx */



#endif /* INCLUDE_RTCM_RTCM2_H_ */
