#
# Autogenerated by Thrift Compiler (0.9.1)
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#
#  options string: py
#

from thrift.Thrift import TType, TMessageType, TException, TApplicationException

from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol, TProtocol
try:
  from thrift.protocol import fastbinary
except:
  fastbinary = None


class ab_status:
  RC_STATUS_OK = 0
  RC_SERVER_ERROR = 1
  RC_SERVER_FAULT = 2
  RC_UNKNOW_ERROR = 3

  _VALUES_TO_NAMES = {
    0: "RC_STATUS_OK",
    1: "RC_SERVER_ERROR",
    2: "RC_SERVER_FAULT",
    3: "RC_UNKNOW_ERROR",
  }

  _NAMES_TO_VALUES = {
    "RC_STATUS_OK": 0,
    "RC_SERVER_ERROR": 1,
    "RC_SERVER_FAULT": 2,
    "RC_UNKNOW_ERROR": 3,
  }


class ABDyeingRequest:
  """
  Attributes:
   - app
   - app_ver
   - _types
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'app', None, None, ), # 1
    (2, TType.STRING, 'app_ver', None, None, ), # 2
    (3, TType.LIST, '_types', (TType.STRING,None), None, ), # 3
  )

  def __init__(self, app=None, app_ver=None, _types=None,):
    self.app = app
    self.app_ver = app_ver
    self._types = _types

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.app = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRING:
          self.app_ver = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 3:
        if ftype == TType.LIST:
          self._types = []
          (_etype3, _size0) = iprot.readListBegin()
          for _i4 in xrange(_size0):
            _elem5 = iprot.readString();
            self._types.append(_elem5)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingRequest')
    if self.app is not None:
      oprot.writeFieldBegin('app', TType.STRING, 1)
      oprot.writeString(self.app)
      oprot.writeFieldEnd()
    if self.app_ver is not None:
      oprot.writeFieldBegin('app_ver', TType.STRING, 2)
      oprot.writeString(self.app_ver)
      oprot.writeFieldEnd()
    if self._types is not None:
      oprot.writeFieldBegin('_types', TType.LIST, 3)
      oprot.writeListBegin(TType.STRING, len(self._types))
      for iter6 in self._types:
        oprot.writeString(iter6)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.app is None:
      raise TProtocol.TProtocolException(message='Required field app is unset!')
    if self.app_ver is None:
      raise TProtocol.TProtocolException(message='Required field app_ver is unset!')
    if self._types is None:
      raise TProtocol.TProtocolException(message='Required field _types is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingLanRequest:
  """
  Attributes:
   - app
   - app_ver
   - app_lan
   - _types
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'app', None, None, ), # 1
    (2, TType.STRING, 'app_ver', None, None, ), # 2
    (3, TType.STRING, 'app_lan', None, None, ), # 3
    (4, TType.LIST, '_types', (TType.STRING,None), None, ), # 4
  )

  def __init__(self, app=None, app_ver=None, app_lan=None, _types=None,):
    self.app = app
    self.app_ver = app_ver
    self.app_lan = app_lan
    self._types = _types

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.app = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRING:
          self.app_ver = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 3:
        if ftype == TType.STRING:
          self.app_lan = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 4:
        if ftype == TType.LIST:
          self._types = []
          (_etype10, _size7) = iprot.readListBegin()
          for _i11 in xrange(_size7):
            _elem12 = iprot.readString();
            self._types.append(_elem12)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingLanRequest')
    if self.app is not None:
      oprot.writeFieldBegin('app', TType.STRING, 1)
      oprot.writeString(self.app)
      oprot.writeFieldEnd()
    if self.app_ver is not None:
      oprot.writeFieldBegin('app_ver', TType.STRING, 2)
      oprot.writeString(self.app_ver)
      oprot.writeFieldEnd()
    if self.app_lan is not None:
      oprot.writeFieldBegin('app_lan', TType.STRING, 3)
      oprot.writeString(self.app_lan)
      oprot.writeFieldEnd()
    if self._types is not None:
      oprot.writeFieldBegin('_types', TType.LIST, 4)
      oprot.writeListBegin(TType.STRING, len(self._types))
      for iter13 in self._types:
        oprot.writeString(iter13)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.app is None:
      raise TProtocol.TProtocolException(message='Required field app is unset!')
    if self.app_ver is None:
      raise TProtocol.TProtocolException(message='Required field app_ver is unset!')
    if self.app_lan is None:
      raise TProtocol.TProtocolException(message='Required field app_lan is unset!')
    if self._types is None:
      raise TProtocol.TProtocolException(message='Required field _types is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingUidReqInfo:
  """
  Attributes:
   - uid
   - _uid_info
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'uid', None, None, ), # 1
    (2, TType.STRUCT, '_uid_info', (ABDyeingRequest, ABDyeingRequest.thrift_spec), None, ), # 2
  )

  def __init__(self, uid=None, _uid_info=None,):
    self.uid = uid
    self._uid_info = _uid_info

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.uid = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRUCT:
          self._uid_info = ABDyeingRequest()
          self._uid_info.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingUidReqInfo')
    if self.uid is not None:
      oprot.writeFieldBegin('uid', TType.STRING, 1)
      oprot.writeString(self.uid)
      oprot.writeFieldEnd()
    if self._uid_info is not None:
      oprot.writeFieldBegin('_uid_info', TType.STRUCT, 2)
      self._uid_info.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.uid is None:
      raise TProtocol.TProtocolException(message='Required field uid is unset!')
    if self._uid_info is None:
      raise TProtocol.TProtocolException(message='Required field _uid_info is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingUidLanReqInfo:
  """
  Attributes:
   - uid
   - _uid_lan_info
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'uid', None, None, ), # 1
    (2, TType.STRUCT, '_uid_lan_info', (ABDyeingLanRequest, ABDyeingLanRequest.thrift_spec), None, ), # 2
  )

  def __init__(self, uid=None, _uid_lan_info=None,):
    self.uid = uid
    self._uid_lan_info = _uid_lan_info

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.uid = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRUCT:
          self._uid_lan_info = ABDyeingLanRequest()
          self._uid_lan_info.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingUidLanReqInfo')
    if self.uid is not None:
      oprot.writeFieldBegin('uid', TType.STRING, 1)
      oprot.writeString(self.uid)
      oprot.writeFieldEnd()
    if self._uid_lan_info is not None:
      oprot.writeFieldBegin('_uid_lan_info', TType.STRUCT, 2)
      self._uid_lan_info.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.uid is None:
      raise TProtocol.TProtocolException(message='Required field uid is unset!')
    if self._uid_lan_info is None:
      raise TProtocol.TProtocolException(message='Required field _uid_lan_info is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingBatchRequest:
  """
  Attributes:
   - _uid_req_infos
  """

  thrift_spec = (
    None, # 0
    (1, TType.LIST, '_uid_req_infos', (TType.STRUCT,(ABDyeingUidReqInfo, ABDyeingUidReqInfo.thrift_spec)), None, ), # 1
  )

  def __init__(self, _uid_req_infos=None,):
    self._uid_req_infos = _uid_req_infos

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.LIST:
          self._uid_req_infos = []
          (_etype17, _size14) = iprot.readListBegin()
          for _i18 in xrange(_size14):
            _elem19 = ABDyeingUidReqInfo()
            _elem19.read(iprot)
            self._uid_req_infos.append(_elem19)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingBatchRequest')
    if self._uid_req_infos is not None:
      oprot.writeFieldBegin('_uid_req_infos', TType.LIST, 1)
      oprot.writeListBegin(TType.STRUCT, len(self._uid_req_infos))
      for iter20 in self._uid_req_infos:
        iter20.write(oprot)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self._uid_req_infos is None:
      raise TProtocol.TProtocolException(message='Required field _uid_req_infos is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingBatchLanRequest:
  """
  Attributes:
   - _uid_lan_req_infos
  """

  thrift_spec = (
    None, # 0
    (1, TType.LIST, '_uid_lan_req_infos', (TType.STRUCT,(ABDyeingUidLanReqInfo, ABDyeingUidLanReqInfo.thrift_spec)), None, ), # 1
  )

  def __init__(self, _uid_lan_req_infos=None,):
    self._uid_lan_req_infos = _uid_lan_req_infos

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.LIST:
          self._uid_lan_req_infos = []
          (_etype24, _size21) = iprot.readListBegin()
          for _i25 in xrange(_size21):
            _elem26 = ABDyeingUidLanReqInfo()
            _elem26.read(iprot)
            self._uid_lan_req_infos.append(_elem26)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingBatchLanRequest')
    if self._uid_lan_req_infos is not None:
      oprot.writeFieldBegin('_uid_lan_req_infos', TType.LIST, 1)
      oprot.writeListBegin(TType.STRUCT, len(self._uid_lan_req_infos))
      for iter27 in self._uid_lan_req_infos:
        iter27.write(oprot)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self._uid_lan_req_infos is None:
      raise TProtocol.TProtocolException(message='Required field _uid_lan_req_infos is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingEntry:
  """
  Attributes:
   - type
   - config_id
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'type', None, None, ), # 1
    (2, TType.STRING, 'config_id', None, None, ), # 2
  )

  def __init__(self, type=None, config_id=None,):
    self.type = type
    self.config_id = config_id

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.type = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRING:
          self.config_id = iprot.readString();
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingEntry')
    if self.type is not None:
      oprot.writeFieldBegin('type', TType.STRING, 1)
      oprot.writeString(self.type)
      oprot.writeFieldEnd()
    if self.config_id is not None:
      oprot.writeFieldBegin('config_id', TType.STRING, 2)
      oprot.writeString(self.config_id)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.type is None:
      raise TProtocol.TProtocolException(message='Required field type is unset!')
    if self.config_id is None:
      raise TProtocol.TProtocolException(message='Required field config_id is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingReply:
  """
  Attributes:
   - status
   - _entries
  """

  thrift_spec = (
    None, # 0
    (1, TType.I32, 'status', None, None, ), # 1
    (2, TType.LIST, '_entries', (TType.STRUCT,(ABDyeingEntry, ABDyeingEntry.thrift_spec)), None, ), # 2
  )

  def __init__(self, status=None, _entries=None,):
    self.status = status
    self._entries = _entries

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.I32:
          self.status = iprot.readI32();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.LIST:
          self._entries = []
          (_etype31, _size28) = iprot.readListBegin()
          for _i32 in xrange(_size28):
            _elem33 = ABDyeingEntry()
            _elem33.read(iprot)
            self._entries.append(_elem33)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingReply')
    if self.status is not None:
      oprot.writeFieldBegin('status', TType.I32, 1)
      oprot.writeI32(self.status)
      oprot.writeFieldEnd()
    if self._entries is not None:
      oprot.writeFieldBegin('_entries', TType.LIST, 2)
      oprot.writeListBegin(TType.STRUCT, len(self._entries))
      for iter34 in self._entries:
        iter34.write(oprot)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.status is None:
      raise TProtocol.TProtocolException(message='Required field status is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingUidRepInfo:
  """
  Attributes:
   - uid
   - _uid_rep
  """

  thrift_spec = (
    None, # 0
    (1, TType.STRING, 'uid', None, None, ), # 1
    (2, TType.STRUCT, '_uid_rep', (ABDyeingReply, ABDyeingReply.thrift_spec), None, ), # 2
  )

  def __init__(self, uid=None, _uid_rep=None,):
    self.uid = uid
    self._uid_rep = _uid_rep

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.STRING:
          self.uid = iprot.readString();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.STRUCT:
          self._uid_rep = ABDyeingReply()
          self._uid_rep.read(iprot)
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingUidRepInfo')
    if self.uid is not None:
      oprot.writeFieldBegin('uid', TType.STRING, 1)
      oprot.writeString(self.uid)
      oprot.writeFieldEnd()
    if self._uid_rep is not None:
      oprot.writeFieldBegin('_uid_rep', TType.STRUCT, 2)
      self._uid_rep.write(oprot)
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.uid is None:
      raise TProtocol.TProtocolException(message='Required field uid is unset!')
    if self._uid_rep is None:
      raise TProtocol.TProtocolException(message='Required field _uid_rep is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)

class ABDyeingBatchReply:
  """
  Attributes:
   - status
   - _uid_rep_infos
  """

  thrift_spec = (
    None, # 0
    (1, TType.I32, 'status', None, None, ), # 1
    (2, TType.LIST, '_uid_rep_infos', (TType.STRUCT,(ABDyeingUidRepInfo, ABDyeingUidRepInfo.thrift_spec)), None, ), # 2
  )

  def __init__(self, status=None, _uid_rep_infos=None,):
    self.status = status
    self._uid_rep_infos = _uid_rep_infos

  def read(self, iprot):
    if iprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and isinstance(iprot.trans, TTransport.CReadableTransport) and self.thrift_spec is not None and fastbinary is not None:
      fastbinary.decode_binary(self, iprot.trans, (self.__class__, self.thrift_spec))
      return
    iprot.readStructBegin()
    while True:
      (fname, ftype, fid) = iprot.readFieldBegin()
      if ftype == TType.STOP:
        break
      if fid == 1:
        if ftype == TType.I32:
          self.status = iprot.readI32();
        else:
          iprot.skip(ftype)
      elif fid == 2:
        if ftype == TType.LIST:
          self._uid_rep_infos = []
          (_etype38, _size35) = iprot.readListBegin()
          for _i39 in xrange(_size35):
            _elem40 = ABDyeingUidRepInfo()
            _elem40.read(iprot)
            self._uid_rep_infos.append(_elem40)
          iprot.readListEnd()
        else:
          iprot.skip(ftype)
      else:
        iprot.skip(ftype)
      iprot.readFieldEnd()
    iprot.readStructEnd()

  def write(self, oprot):
    if oprot.__class__ == TBinaryProtocol.TBinaryProtocolAccelerated and self.thrift_spec is not None and fastbinary is not None:
      oprot.trans.write(fastbinary.encode_binary(self, (self.__class__, self.thrift_spec)))
      return
    oprot.writeStructBegin('ABDyeingBatchReply')
    if self.status is not None:
      oprot.writeFieldBegin('status', TType.I32, 1)
      oprot.writeI32(self.status)
      oprot.writeFieldEnd()
    if self._uid_rep_infos is not None:
      oprot.writeFieldBegin('_uid_rep_infos', TType.LIST, 2)
      oprot.writeListBegin(TType.STRUCT, len(self._uid_rep_infos))
      for iter41 in self._uid_rep_infos:
        iter41.write(oprot)
      oprot.writeListEnd()
      oprot.writeFieldEnd()
    oprot.writeFieldStop()
    oprot.writeStructEnd()

  def validate(self):
    if self.status is None:
      raise TProtocol.TProtocolException(message='Required field status is unset!')
    return


  def __repr__(self):
    L = ['%s=%r' % (key, value)
      for key, value in self.__dict__.iteritems()]
    return '%s(%s)' % (self.__class__.__name__, ', '.join(L))

  def __eq__(self, other):
    return isinstance(other, self.__class__) and self.__dict__ == other.__dict__

  def __ne__(self, other):
    return not (self == other)
