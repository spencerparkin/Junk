// PerforceOperation.h

#pragma once

#include "p4/clientapi.h"
#include "wxHeaders.h"

//=================================================================================
class PerforceOperation : public ClientUser
{
public:

	PerforceOperation( void );
	virtual ~PerforceOperation( void );

	virtual bool Perform( void ) = 0;

	void SetFile( const wxString& file );
	wxString GetFile( void ) const;

protected:

	bool InitializeClient( const wxString* file = 0 );
	bool FinalizeClient( void );
	bool IssueCommand( const wxArrayString& command );
	bool InitIssueFinal( const wxArrayString& command, const wxString* file = 0 );
	bool ShowError( Error* error );

	virtual void OutputStat( StrDict* varList ) override;
	virtual void OutputError( const char* error ) override;

	ClientApi* client;

	WX_DECLARE_STRING_HASH_MAP( wxString, OperationResults );
	virtual void ProcessResults( OperationResults& operationResults ) = 0;

	wxString file, userName;
};

//=================================================================================
class FileStatusOperation : public PerforceOperation
{
public:

	struct FileStatus
	{
		bool unknown;
		bool underSourceControl;
		bool haveLatestRevision;
		bool lockedByMe;
		bool lockedBySomeoneElse;
		bool openBySomeoneElse;
		// TODO: Add string array giving who else has it open.
		enum OpenFor { NOT_OPEN, OPEN_FOR_ADD, OPEN_FOR_EDIT, OPEN_FOR_DELETE };
		OpenFor openFor;
	};

	FileStatusOperation( void );
	virtual ~FileStatusOperation( void );

	virtual bool Perform( void ) override;

protected:

	virtual void ProcessResults( OperationResults& operationResults ) override;

public:

	FileStatus fileStatus;
};

//=================================================================================
class AddOperation : public PerforceOperation
{
public:

	AddOperation( void );
	virtual ~AddOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class RevertOperation : public PerforceOperation
{
public:

	RevertOperation( void );
	virtual ~RevertOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class OpenForEditOperation : public PerforceOperation
{
public:

	OpenForEditOperation( void );
	virtual ~OpenForEditOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class SubmitOperation : public PerforceOperation
{
public:

	SubmitOperation( void );
	virtual ~SubmitOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class GetLatestOperation : public PerforceOperation
{
public:

	GetLatestOperation( void );
	virtual ~GetLatestOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class LockOperation : public PerforceOperation
{
public:

	LockOperation( void );
	virtual ~LockOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

//=================================================================================
class UnlockOperation : public PerforceOperation
{
public:

	UnlockOperation( void );
	virtual ~UnlockOperation( void );

	virtual bool Perform( void ) override;

	virtual void ProcessResults( OperationResults& operationResults ) override;
};

// PerforceOperation.h