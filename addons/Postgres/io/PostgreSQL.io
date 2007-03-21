

Postgres := Object clone 
Postgres do(
	type := "Postgres"
	docCategory("Databases")
	
	enum := method(
		i := 0
		msg := call message clone
		prt := call sender doMessage(msg argAt(0)) clone
		prefix := call sender doMessage(msg argAt(1))
		
		msg arguments select(idx, val, idx > 1) foreach(arg,
		
			if(arg name == "updateSlot") then(
				args := arg argsEvaluatedIn(call sender)
				value := args at(1)
				slot := args at(0)
			) else(
				value := i; slot := arg asString
			)
			
			slot := slot asMutable removePrefix(prefix)
			i := prt setSlot(slot, value) + 1
			//   (slot .. ":=" ..(value asString) .. "\n") print
		)
		prt
	)
	  
	PGconn   := Postgres clone
	PGresult := Postgres clone
	PGnumber := Postgres clone
	PGstring := Postgres clone
	void := Postgres clone
	
	ConnStatusType := enum(PGnumber, "CONNECTION_",
			// Although it is okay to add to this list, values which become unused
			// should never be removed, nor should constants be redefined - that
			// would break compatibility with existing code.
	
			CONNECTION_OK,
			CONNECTION_BAD,
			// Non-blocking mode only below here 
	
			// The existence of these should never be relied upon - they should
			// only be used for user feedback or similar purposes.
	
			CONNECTION_STARTED,                     // Waiting for connection to be made.
			CONNECTION_MADE,                        // Connection OK; waiting to send.
			CONNECTION_AWAITING_RESPONSE,           // Waiting for a response from the postmaster.          
			CONNECTION_AUTH_OK,                     // Received authentication; waiting for backend startup.
			CONNECTION_SETENV,                      // Negotiating environment.
			CONNECTION_SSL_STARTUP,         // Negotiating SSL.
			CONNECTION_NEEDED                       // Internal state: connect() needed 
	)
	
	PostgresPollingStatusType := enum(PGnumber, "PGRES_POLLING_",
			PGRES_POLLING_FAILED = 0,
			PGRES_POLLING_READING,          // These two indicate that one may
			PGRES_POLLING_WRITING,          // use select before polling again.
			PGRES_POLLING_OK,
			PGRES_POLLING_ACTIVE            // unused; keep for awhile for backwards compatibility 
	)
	
	ExecStatusType := enum(PGnumber, "PGRES_",
			PGRES_EMPTY_QUERY = 0,          // empty query string was executed
			PGRES_COMMAND_OK,                       
			// a query command that doesn't return  anything was executed properly by the backend
			
			PGRES_TUPLES_OK,                        
			// a query command that returns tuples was executed properly by the backend, PGresult contains the result tuples
			PGRES_COPY_OUT,                         // Copy Out data transfer in progress
			PGRES_COPY_IN,                          // Copy In data transfer in progress
			PGRES_BAD_RESPONSE,                     // an unexpected response was recv'd from  the backend
			PGRES_NONFATAL_ERROR,           // notice or warning message
			PGRES_FATAL_ERROR                       // query failed
	)
	
	PGTransactionStatusType := enum(PGnumber, "PQTRANS_",
			PQTRANS_IDLE,                           // connection idle
			PQTRANS_ACTIVE,                         // command in progress
			PQTRANS_INTRANS,                        // idle, within transaction block
			PQTRANS_INERROR,                        // idle, within failed transaction
			PQTRANS_UNKNOWN                         // cannot determine status
	)
	
	PGVerbosity := enum(PGnumber, "PQERRORS_",
			PQERRORS_TERSE,                         // single-line error messages
			PQERRORS_DEFAULT,                       // recommended style
			PQERRORS_VERBOSE                        // all the facts, ma'am
	)
	
	PGnumber fromPostgres := method(obj, obj pgobject)
	PGstring fromPostgres := method(obj, libpq returnsString(obj pgobject))

	libpq := method(
		if(Addon platform == "darwin",
			self libpq := DynLib clone open("/opt/local/lib/postgresql81/libpq.dylib"),
			self libpq := DynLib clone open("libpq." .. Addon dllSuffix)
		)
	)
	
	forward := method(
		args := call message argsEvaluatedIn(call sender)
		name := call message name
		if(?pgobject, args prepend(self pgobject))
		t := Postgres clone 
		callType := if(map at(name) == void, "voidCall", "call")
		t pgobject := libpq performWithArgList(callType, args prepend("PQ" .. name))
		self convert(name, t)
	)
	
	fromPostgres := method(obj, obj setProto(self); obj)
	map := Map clone
	returns := method(name, converter, map atPut(name, converter); self)
	convert := method(name, obj, map at(name) fromPostgres(obj))
	
	PGconn do(
		returns("connectdb", PGconn)
		returns("connectStart", PGconn)
		returns("setdbLogin", PGconn)
		returns("backendPID", PGnumber)
		returns("clientEncoding", PGnumber)
		returns("consumeInput", PGnumber)
		returns("db", PGstring)
		returns("endcopy", PGnumber)
		returns("errorMessage", PGstring)
		returns("execParams", PGresult)
		returns("exec", PGresult)
		returns("execPrepared", PGresult)
		returns("flush", PGnumber)
		returns("fn", PGresult)
		returns("getCopyData", PGnumber)
		returns("getlineAsync", PGnumber)
		returns("getline", PGnumber)
		returns("getResult", PGresult)
		returns("host", PGstring)
		returns("isBusy", PGnumber)
		returns("isnonblocking", PGnumber)
		returns("makeEmptyPGresult", PGresult)
		returns("options", PGstring)
		returns("pass", PGstring)
		returns("port", PGstring)
		returns("protocolVersion", PGnumber)
		returns("putCopyData", PGnumber)
		returns("putCopyEnd", PGnumber)
		returns("putline", PGnumber)
		returns("putnbytes", PGnumber)
		returns("requestCancel", PGnumber)
		returns("resetStart", PGnumber)
		returns("sendQueryParams", PGnumber)
		returns("sendQuery", PGnumber)
		returns("sendQueryPrepared", PGnumber)
		returns("setClientEncoding", PGnumber)
		returns("setnonblocking", PGnumber)
		returns("socket", PGnumber)
		returns("tty", PGstring)
		returns("user", PGstring)
		returns("connectPoll", PostgresPollingStatusType)
		returns("resetPoll", PostgresPollingStatusType)
		returns("status", ConnStatusType)
		returns("transactionStatus", PGTransactionStatusType)
		returns("setErrorVerbosity", PGVerbosity)
		returns("finish", void)
		returns("reset", void)
		returns("trace", void)
		returns("untrace", void)
	)
	
	PGresult do(
		returns("binaryTuples", PGnumber)
		returns("cmdStatus", PGstring)
		returns("cmdTuples", PGstring)
		returns("fformat", PGnumber)
		returns("fmod", PGnumber)
		returns("fname", PGstring)
		returns("fnumber", PGnumber)
		returns("fsize", PGnumber)
		returns("ftablecol", PGnumber)
		returns("getisnull", PGnumber)
		returns("getlength", PGnumber)
		returns("getvalue", PGstring)
		returns("nfields", PGnumber)
		returns("ntuples", PGnumber)
		returns("oidStatus", PGstring)
		returns("resultErrorField", PGstring)
		returns("resultErrorMessage", PGstring)
		//ExecStatusType returns("resStatus", PGstring)
		returns("resultStatus", ExecStatusType)
		returns("clear", void)
		//PQconninfoOption returns("conninfoFree", void)
	)
)

