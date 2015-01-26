// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: pkgheader.proto

#ifndef PROTOBUF_pkgheader_2eproto__INCLUDED
#define PROTOBUF_pkgheader_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace quote {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_pkgheader_2eproto();
void protobuf_AssignDesc_pkgheader_2eproto();
void protobuf_ShutdownFile_pkgheader_2eproto();

class PkgHeader;

enum PkgHeader_MsgType {
  PkgHeader_MsgType_REQ_REALTIME = 1,
  PkgHeader_MsgType_RES_REALTIME = 2,
  PkgHeader_MsgType_REQ_GRID = 3,
  PkgHeader_MsgType_RES_GRID = 4,
  PkgHeader_MsgType_REQ_GRIDSORT = 5,
  PkgHeader_MsgType_RES_GRIDSORT = 6,
  PkgHeader_MsgType_REQ_DETAIL = 7,
  PkgHeader_MsgType_RES_DETAIL = 8,
  PkgHeader_MsgType_REQ_MIN = 9,
  PkgHeader_MsgType_RES_MIN = 10,
  PkgHeader_MsgType_REQ_STATUS = 11,
  PkgHeader_MsgType_RES_STATUS = 12,
  PkgHeader_MsgType_REQ_ZHPM = 13,
  PkgHeader_MsgType_RES_ZHPM = 14,
  PkgHeader_MsgType_REQ_CANDLE = 15,
  PkgHeader_MsgType_RES_CANDLE = 16,
  PkgHeader_MsgType_REQ_STOCKS = 17,
  PkgHeader_MsgType_RES_STOCKS = 18,
  PkgHeader_MsgType_REQ_FINANCE = 19,
  PkgHeader_MsgType_RES_FINANCE = 20,
  PkgHeader_MsgType_REQ_WEIGHT = 21,
  PkgHeader_MsgType_RES_WEIGHT = 22,
  PkgHeader_MsgType_REQ_ZSBX = 23,
  PkgHeader_MsgType_RES_ZSBX = 24,
  PkgHeader_MsgType_REQ_BLOCK = 25,
  PkgHeader_MsgType_RES_BLOCK = 26,
  PkgHeader_MsgType_REQ_MD5 = 27,
  PkgHeader_MsgType_RES_MD5 = 28,
  PkgHeader_MsgType_REQ_LARGE = 29,
  PkgHeader_MsgType_RES_LARGE = 30,
  PkgHeader_MsgType_REQ_PRICE = 31,
  PkgHeader_MsgType_RES_PRICE = 32,
  PkgHeader_MsgType_REQ_TRADE = 33,
  PkgHeader_MsgType_RES_TRADE = 34,
  PkgHeader_MsgType_REQ_CAPTCHA = 35,
  PkgHeader_MsgType_RES_CAPTCHA = 36,
  PkgHeader_MsgType_REQ_OCR = 37,
  PkgHeader_MsgType_RES_OCR = 38
};
bool PkgHeader_MsgType_IsValid(int value);
const PkgHeader_MsgType PkgHeader_MsgType_MsgType_MIN = PkgHeader_MsgType_REQ_REALTIME;
const PkgHeader_MsgType PkgHeader_MsgType_MsgType_MAX = PkgHeader_MsgType_RES_OCR;
const int PkgHeader_MsgType_MsgType_ARRAYSIZE = PkgHeader_MsgType_MsgType_MAX + 1;

const ::google::protobuf::EnumDescriptor* PkgHeader_MsgType_descriptor();
inline const ::std::string& PkgHeader_MsgType_Name(PkgHeader_MsgType value) {
  return ::google::protobuf::internal::NameOfEnum(
    PkgHeader_MsgType_descriptor(), value);
}
inline bool PkgHeader_MsgType_Parse(
    const ::std::string& name, PkgHeader_MsgType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PkgHeader_MsgType>(
    PkgHeader_MsgType_descriptor(), name, value);
}
// ===================================================================

class PkgHeader : public ::google::protobuf::Message {
 public:
  PkgHeader();
  virtual ~PkgHeader();

  PkgHeader(const PkgHeader& from);

  inline PkgHeader& operator=(const PkgHeader& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PkgHeader& default_instance();

  void Swap(PkgHeader* other);

  // implements Message ----------------------------------------------

  PkgHeader* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PkgHeader& from);
  void MergeFrom(const PkgHeader& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef PkgHeader_MsgType MsgType;
  static const MsgType REQ_REALTIME = PkgHeader_MsgType_REQ_REALTIME;
  static const MsgType RES_REALTIME = PkgHeader_MsgType_RES_REALTIME;
  static const MsgType REQ_GRID = PkgHeader_MsgType_REQ_GRID;
  static const MsgType RES_GRID = PkgHeader_MsgType_RES_GRID;
  static const MsgType REQ_GRIDSORT = PkgHeader_MsgType_REQ_GRIDSORT;
  static const MsgType RES_GRIDSORT = PkgHeader_MsgType_RES_GRIDSORT;
  static const MsgType REQ_DETAIL = PkgHeader_MsgType_REQ_DETAIL;
  static const MsgType RES_DETAIL = PkgHeader_MsgType_RES_DETAIL;
  static const MsgType REQ_MIN = PkgHeader_MsgType_REQ_MIN;
  static const MsgType RES_MIN = PkgHeader_MsgType_RES_MIN;
  static const MsgType REQ_STATUS = PkgHeader_MsgType_REQ_STATUS;
  static const MsgType RES_STATUS = PkgHeader_MsgType_RES_STATUS;
  static const MsgType REQ_ZHPM = PkgHeader_MsgType_REQ_ZHPM;
  static const MsgType RES_ZHPM = PkgHeader_MsgType_RES_ZHPM;
  static const MsgType REQ_CANDLE = PkgHeader_MsgType_REQ_CANDLE;
  static const MsgType RES_CANDLE = PkgHeader_MsgType_RES_CANDLE;
  static const MsgType REQ_STOCKS = PkgHeader_MsgType_REQ_STOCKS;
  static const MsgType RES_STOCKS = PkgHeader_MsgType_RES_STOCKS;
  static const MsgType REQ_FINANCE = PkgHeader_MsgType_REQ_FINANCE;
  static const MsgType RES_FINANCE = PkgHeader_MsgType_RES_FINANCE;
  static const MsgType REQ_WEIGHT = PkgHeader_MsgType_REQ_WEIGHT;
  static const MsgType RES_WEIGHT = PkgHeader_MsgType_RES_WEIGHT;
  static const MsgType REQ_ZSBX = PkgHeader_MsgType_REQ_ZSBX;
  static const MsgType RES_ZSBX = PkgHeader_MsgType_RES_ZSBX;
  static const MsgType REQ_BLOCK = PkgHeader_MsgType_REQ_BLOCK;
  static const MsgType RES_BLOCK = PkgHeader_MsgType_RES_BLOCK;
  static const MsgType REQ_MD5 = PkgHeader_MsgType_REQ_MD5;
  static const MsgType RES_MD5 = PkgHeader_MsgType_RES_MD5;
  static const MsgType REQ_LARGE = PkgHeader_MsgType_REQ_LARGE;
  static const MsgType RES_LARGE = PkgHeader_MsgType_RES_LARGE;
  static const MsgType REQ_PRICE = PkgHeader_MsgType_REQ_PRICE;
  static const MsgType RES_PRICE = PkgHeader_MsgType_RES_PRICE;
  static const MsgType REQ_TRADE = PkgHeader_MsgType_REQ_TRADE;
  static const MsgType RES_TRADE = PkgHeader_MsgType_RES_TRADE;
  static const MsgType REQ_CAPTCHA = PkgHeader_MsgType_REQ_CAPTCHA;
  static const MsgType RES_CAPTCHA = PkgHeader_MsgType_RES_CAPTCHA;
  static const MsgType REQ_OCR = PkgHeader_MsgType_REQ_OCR;
  static const MsgType RES_OCR = PkgHeader_MsgType_RES_OCR;
  static inline bool MsgType_IsValid(int value) {
    return PkgHeader_MsgType_IsValid(value);
  }
  static const MsgType MsgType_MIN =
    PkgHeader_MsgType_MsgType_MIN;
  static const MsgType MsgType_MAX =
    PkgHeader_MsgType_MsgType_MAX;
  static const int MsgType_ARRAYSIZE =
    PkgHeader_MsgType_MsgType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  MsgType_descriptor() {
    return PkgHeader_MsgType_descriptor();
  }
  static inline const ::std::string& MsgType_Name(MsgType value) {
    return PkgHeader_MsgType_Name(value);
  }
  static inline bool MsgType_Parse(const ::std::string& name,
      MsgType* value) {
    return PkgHeader_MsgType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required fixed32 bodysize = 1;
  inline bool has_bodysize() const;
  inline void clear_bodysize();
  static const int kBodysizeFieldNumber = 1;
  inline ::google::protobuf::uint32 bodysize() const;
  inline void set_bodysize(::google::protobuf::uint32 value);

  // required fixed32 ver = 2;
  inline bool has_ver() const;
  inline void clear_ver();
  static const int kVerFieldNumber = 2;
  inline ::google::protobuf::uint32 ver() const;
  inline void set_ver(::google::protobuf::uint32 value);

  // required bool enc = 3;
  inline bool has_enc() const;
  inline void clear_enc();
  static const int kEncFieldNumber = 3;
  inline bool enc() const;
  inline void set_enc(bool value);

  // optional bool zip = 4;
  inline bool has_zip() const;
  inline void clear_zip();
  static const int kZipFieldNumber = 4;
  inline bool zip() const;
  inline void set_zip(bool value);

  // optional bool more = 5;
  inline bool has_more() const;
  inline void clear_more();
  static const int kMoreFieldNumber = 5;
  inline bool more() const;
  inline void set_more(bool value);

  // required fixed32 errcode = 6;
  inline bool has_errcode() const;
  inline void clear_errcode();
  static const int kErrcodeFieldNumber = 6;
  inline ::google::protobuf::uint32 errcode() const;
  inline void set_errcode(::google::protobuf::uint32 value);

  // required .quote.PkgHeader.MsgType msgtype = 7;
  inline bool has_msgtype() const;
  inline void clear_msgtype();
  static const int kMsgtypeFieldNumber = 7;
  inline ::quote::PkgHeader_MsgType msgtype() const;
  inline void set_msgtype(::quote::PkgHeader_MsgType value);

  // @@protoc_insertion_point(class_scope:quote.PkgHeader)
 private:
  inline void set_has_bodysize();
  inline void clear_has_bodysize();
  inline void set_has_ver();
  inline void clear_has_ver();
  inline void set_has_enc();
  inline void clear_has_enc();
  inline void set_has_zip();
  inline void clear_has_zip();
  inline void set_has_more();
  inline void clear_has_more();
  inline void set_has_errcode();
  inline void clear_has_errcode();
  inline void set_has_msgtype();
  inline void clear_has_msgtype();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 bodysize_;
  ::google::protobuf::uint32 ver_;
  bool enc_;
  bool zip_;
  bool more_;
  ::google::protobuf::uint32 errcode_;
  int msgtype_;
  friend void  protobuf_AddDesc_pkgheader_2eproto();
  friend void protobuf_AssignDesc_pkgheader_2eproto();
  friend void protobuf_ShutdownFile_pkgheader_2eproto();

  void InitAsDefaultInstance();
  static PkgHeader* default_instance_;
};
// ===================================================================


// ===================================================================

// PkgHeader

// required fixed32 bodysize = 1;
inline bool PkgHeader::has_bodysize() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PkgHeader::set_has_bodysize() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PkgHeader::clear_has_bodysize() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PkgHeader::clear_bodysize() {
  bodysize_ = 0u;
  clear_has_bodysize();
}
inline ::google::protobuf::uint32 PkgHeader::bodysize() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.bodysize)
  return bodysize_;
}
inline void PkgHeader::set_bodysize(::google::protobuf::uint32 value) {
  set_has_bodysize();
  bodysize_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.bodysize)
}

// required fixed32 ver = 2;
inline bool PkgHeader::has_ver() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PkgHeader::set_has_ver() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PkgHeader::clear_has_ver() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PkgHeader::clear_ver() {
  ver_ = 0u;
  clear_has_ver();
}
inline ::google::protobuf::uint32 PkgHeader::ver() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.ver)
  return ver_;
}
inline void PkgHeader::set_ver(::google::protobuf::uint32 value) {
  set_has_ver();
  ver_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.ver)
}

// required bool enc = 3;
inline bool PkgHeader::has_enc() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PkgHeader::set_has_enc() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PkgHeader::clear_has_enc() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PkgHeader::clear_enc() {
  enc_ = false;
  clear_has_enc();
}
inline bool PkgHeader::enc() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.enc)
  return enc_;
}
inline void PkgHeader::set_enc(bool value) {
  set_has_enc();
  enc_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.enc)
}

// optional bool zip = 4;
inline bool PkgHeader::has_zip() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void PkgHeader::set_has_zip() {
  _has_bits_[0] |= 0x00000008u;
}
inline void PkgHeader::clear_has_zip() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void PkgHeader::clear_zip() {
  zip_ = false;
  clear_has_zip();
}
inline bool PkgHeader::zip() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.zip)
  return zip_;
}
inline void PkgHeader::set_zip(bool value) {
  set_has_zip();
  zip_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.zip)
}

// optional bool more = 5;
inline bool PkgHeader::has_more() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void PkgHeader::set_has_more() {
  _has_bits_[0] |= 0x00000010u;
}
inline void PkgHeader::clear_has_more() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void PkgHeader::clear_more() {
  more_ = false;
  clear_has_more();
}
inline bool PkgHeader::more() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.more)
  return more_;
}
inline void PkgHeader::set_more(bool value) {
  set_has_more();
  more_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.more)
}

// required fixed32 errcode = 6;
inline bool PkgHeader::has_errcode() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void PkgHeader::set_has_errcode() {
  _has_bits_[0] |= 0x00000020u;
}
inline void PkgHeader::clear_has_errcode() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void PkgHeader::clear_errcode() {
  errcode_ = 0u;
  clear_has_errcode();
}
inline ::google::protobuf::uint32 PkgHeader::errcode() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.errcode)
  return errcode_;
}
inline void PkgHeader::set_errcode(::google::protobuf::uint32 value) {
  set_has_errcode();
  errcode_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.errcode)
}

// required .quote.PkgHeader.MsgType msgtype = 7;
inline bool PkgHeader::has_msgtype() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void PkgHeader::set_has_msgtype() {
  _has_bits_[0] |= 0x00000040u;
}
inline void PkgHeader::clear_has_msgtype() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void PkgHeader::clear_msgtype() {
  msgtype_ = 1;
  clear_has_msgtype();
}
inline ::quote::PkgHeader_MsgType PkgHeader::msgtype() const {
  // @@protoc_insertion_point(field_get:quote.PkgHeader.msgtype)
  return static_cast< ::quote::PkgHeader_MsgType >(msgtype_);
}
inline void PkgHeader::set_msgtype(::quote::PkgHeader_MsgType value) {
  assert(::quote::PkgHeader_MsgType_IsValid(value));
  set_has_msgtype();
  msgtype_ = value;
  // @@protoc_insertion_point(field_set:quote.PkgHeader.msgtype)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace quote

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::quote::PkgHeader_MsgType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::quote::PkgHeader_MsgType>() {
  return ::quote::PkgHeader_MsgType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_pkgheader_2eproto__INCLUDED
