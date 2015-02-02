// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: user.proto

#ifndef PROTOBUF_user_2eproto__INCLUDED
#define PROTOBUF_user_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace db_msg {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_user_2eproto();
void protobuf_AssignDesc_user_2eproto();
void protobuf_ShutdownFile_user_2eproto();

class MUser;

// ===================================================================

class MUser : public ::google::protobuf::Message {
 public:
  MUser();
  virtual ~MUser();

  MUser(const MUser& from);

  inline MUser& operator=(const MUser& from) {
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
  static const MUser& default_instance();

  void Swap(MUser* other);

  // implements Message ----------------------------------------------

  MUser* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MUser& from);
  void MergeFrom(const MUser& from);
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

  // accessors -------------------------------------------------------

  // required uint32 userid = 1;
  inline bool has_userid() const;
  inline void clear_userid();
  static const int kUseridFieldNumber = 1;
  inline ::google::protobuf::uint32 userid() const;
  inline void set_userid(::google::protobuf::uint32 value);

  // required string name = 2;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 2;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // optional uint32 score = 3;
  inline bool has_score() const;
  inline void clear_score();
  static const int kScoreFieldNumber = 3;
  inline ::google::protobuf::uint32 score() const;
  inline void set_score(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:db_msg.MUser)
 private:
  inline void set_has_userid();
  inline void clear_has_userid();
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_score();
  inline void clear_has_score();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::google::protobuf::uint32 userid_;
  ::google::protobuf::uint32 score_;
  friend void  protobuf_AddDesc_user_2eproto();
  friend void protobuf_AssignDesc_user_2eproto();
  friend void protobuf_ShutdownFile_user_2eproto();

  void InitAsDefaultInstance();
  static MUser* default_instance_;
};
// ===================================================================


// ===================================================================

// MUser

// required uint32 userid = 1;
inline bool MUser::has_userid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MUser::set_has_userid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MUser::clear_has_userid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MUser::clear_userid() {
  userid_ = 0u;
  clear_has_userid();
}
inline ::google::protobuf::uint32 MUser::userid() const {
  // @@protoc_insertion_point(field_get:db_msg.MUser.userid)
  return userid_;
}
inline void MUser::set_userid(::google::protobuf::uint32 value) {
  set_has_userid();
  userid_ = value;
  // @@protoc_insertion_point(field_set:db_msg.MUser.userid)
}

// required string name = 2;
inline bool MUser::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MUser::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MUser::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MUser::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& MUser::name() const {
  // @@protoc_insertion_point(field_get:db_msg.MUser.name)
  return *name_;
}
inline void MUser::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:db_msg.MUser.name)
}
inline void MUser::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:db_msg.MUser.name)
}
inline void MUser::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:db_msg.MUser.name)
}
inline ::std::string* MUser::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:db_msg.MUser.name)
  return name_;
}
inline ::std::string* MUser::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MUser::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:db_msg.MUser.name)
}

// optional uint32 score = 3;
inline bool MUser::has_score() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MUser::set_has_score() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MUser::clear_has_score() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MUser::clear_score() {
  score_ = 0u;
  clear_has_score();
}
inline ::google::protobuf::uint32 MUser::score() const {
  // @@protoc_insertion_point(field_get:db_msg.MUser.score)
  return score_;
}
inline void MUser::set_score(::google::protobuf::uint32 value) {
  set_has_score();
  score_ = value;
  // @@protoc_insertion_point(field_set:db_msg.MUser.score)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace db_msg

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_user_2eproto__INCLUDED
