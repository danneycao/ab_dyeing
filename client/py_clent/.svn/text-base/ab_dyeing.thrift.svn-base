namespace cpp inveno

enum ab_status {
	RC_STATUS_OK 		= 	0,
	RC_SERVER_ERROR 	=	1,
	RC_SERVER_FAULT		= 	2,
	RC_UNKNOW_ERROR		=	3,
}

struct ABDyeingRequest {
	1: required string app,             // 渠道名 
    2: required string app_ver,         // 渠道版本
    3: required list<string> _types,    // 请求的业务类型列表
}

struct ABDyeingLanRequest {
	1: required string app,             // 渠道名 
    2: required string app_ver,         // 渠道版本
    3: required string app_lan,         // 语言
    4: required list<string> _types,    // 请求的业务类型列表
}

struct ABDyeingUidReqInfo {
    1: required string uid,
    2: required ABDyeingRequest _uid_info,
}

struct ABDyeingUidLanReqInfo {
    1: required string uid,
    2: required ABDyeingLanRequest _uid_lan_info,
}

struct ABDyeingBatchRequest {
    1: required list<ABDyeingUidReqInfo> _uid_req_infos,  
}

struct ABDyeingBatchLanRequest {
    1: required list<ABDyeingUidLanReqInfo> _uid_lan_req_infos, 
}

struct ABDyeingEntry {
	1: required string type,        // 业务类型
	2: required string config_id,   // 返回的config_id
}

struct ABDyeingReply {
	1: required ab_status status, 
	2: optional list<ABDyeingEntry> _entries,
}

struct ABDyeingUidRepInfo {
    1: required string uid,
    2: required ABDyeingReply _uid_rep,
}

struct ABDyeingBatchReply {
    1: required ab_status status,
    2: optional list<ABDyeingUidRepInfo> _uid_rep_infos,
}

service ABDyeingService {
	ABDyeingReply dyeing(1:string uid, 2:ABDyeingRequest req),
	
    ABDyeingReply DyeingLan(1:string uid, 2:ABDyeingLanRequest req),

    ABDyeingBatchReply BatchDyeing(1:ABDyeingBatchRequest batch_reqs),
    
    ABDyeingBatchReply BatchDyeingLan(1:ABDyeingBatchLanRequest batch_reqs),
}
