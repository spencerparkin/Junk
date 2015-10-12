// PerforceOperation.cpp

#include "PerforceOperation.h"

//=================================================================================
PerforceOperation::PerforceOperation( void )
{
	client = 0;
}

//=================================================================================
/*virtual*/ PerforceOperation::~PerforceOperation( void )
{
	delete client;
}

//=================================================================================
void PerforceOperation::SetFile( const wxString& file )
{
	this->file = file;
}

//=================================================================================
wxString PerforceOperation::GetFile( void ) const
{
	return file;
}

//=================================================================================
bool PerforceOperation::ShowError( Error* error )
{
	if( error->Test() )
	{
		StrBuf msg;
		error->Fmt( &msg );
		wxString errorString = msg.Text();
		wxMessageBox( wxT( "Perforce operation failure: " ) + errorString, wxT( "Perforce Operation Failure" ), wxOK | wxCENTRE );
		return true;
	}
	return false;
}

//=================================================================================
bool PerforceOperation::InitializeClient( const wxString* file /*= 0*/ )
{
	if( client )
		return false;

	// When we create a new perforce client, perforce tries to auto-detect
	// the depot that we want to work in.  It does this by using the current
	// working directory.  It probably goes up the tree until it find the
	// depot root, so go ahead and use the given file to set the CWD.
	if( file && !file->IsEmpty() )
	{
		wxFileName fileName( *file );
		wxString directory = fileName.GetPath();
		::SetCurrentDirectoryA( ( const char* )directory.c_str() );
	}

	client = new ClientApi();
	client->SetProtocol( "tag", "" );

	Error error;
	client->Init( &error );
	if( ShowError( &error ) )
		return false;

	client->SetProg( "PerforceClient plugin for CornucopiaEditor" );

	userName = client->GetUser().Text();

	return true;
}

//=================================================================================
bool PerforceOperation::FinalizeClient( void )
{
	if( !client )
		return false;

	Error error;
	client->Final( &error );
	if( ShowError( &error ) )
		return false;

	delete client;
	client = 0;
	return true;
}

//=================================================================================
bool PerforceOperation::IssueCommand( const wxArrayString& command )
{
	if( !client )
		return false;

	int argc = command.Count();
	if( argc <= 0 )
		return false;

	char** argv = new char*[ argc ];
	for( int index = 0; index < argc; index++ )
		argv[ index ] = ( char* )( const char* )command[ index ].c_str();
	
	client->SetArgv( argc - 1, ( char* const* )( argv + 1 ) );
	client->Run( argv[0], this );

	delete[] argv;

	if( client->Dropped() )
	{
		//...?
	}

	return true;
}

//=================================================================================
bool PerforceOperation::InitIssueFinal( const wxArrayString& command, const wxString* file /*= 0*/ )
{
	if( !InitializeClient( file ) )
		return false;

	if( !IssueCommand( command ) )
		return false;

	if( !FinalizeClient() )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ void PerforceOperation::OutputStat( StrDict* varList )
{
	StrRef key, value;
	
	OperationResults operationResults;
	for( int index = 0; varList->GetVar( index, key, value ) != 0; index++ )
		operationResults[ key.Text() ] = value.Text();
	
	ProcessResults( operationResults );
}

//=================================================================================
/*virtual*/ void PerforceOperation::OutputError( const char* error )
{
	OperationResults operationResults;
	operationResults[ "p4_error" ] = error;
	ProcessResults( operationResults );
}

//=================================================================================
FileStatusOperation::FileStatusOperation( void )
{
	fileStatus.unknown = true;
}

//=================================================================================
/*virtual*/ FileStatusOperation::~FileStatusOperation( void )
{
}

//=================================================================================
/*virtual*/ bool FileStatusOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "fstat" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void FileStatusOperation::ProcessResults( OperationResults& operationResults )
{
	// In this case, I assume that this means that the file is simply not yet part of the depot.
	OperationResults::iterator iter = operationResults.find( "p4_error" );
	if( iter != operationResults.end() )
	{
		if( -1 != iter->second.find( "no such file" ) )
		{
			fileStatus.unknown = false;
			fileStatus.underSourceControl = false;
			fileStatus.openFor = FileStatus::NOT_OPEN;
			fileStatus.openBySomeoneElse = false;
			fileStatus.haveLatestRevision = true;
			fileStatus.lockedByMe = false;
			fileStatus.lockedBySomeoneElse = false;
			return;
		}
	}

	// In all other cases, the file is either mapped or not mapped, right?
	iter = operationResults.find( "isMapped" );
	if( iter != operationResults.end() )
	{
		fileStatus.unknown = false;
		fileStatus.underSourceControl = true;

		iter = operationResults.find( "action" );
		if( iter == operationResults.end() )
			fileStatus.openFor = FileStatus::NOT_OPEN;
		else if( iter->second == "edit" )
			fileStatus.openFor = FileStatus::OPEN_FOR_EDIT;
		else if( iter->second == "delete" )
			fileStatus.openFor = FileStatus::OPEN_FOR_DELETE;

		iter = operationResults.find( "otherOpen" );
		fileStatus.openBySomeoneElse = ( iter != operationResults.end() ) ? true : false;
		
		iter = operationResults.find( "ourLock" );
		fileStatus.lockedByMe = ( iter != operationResults.end() ) ? true : false;

		iter = operationResults.find( "otherLock" );
		fileStatus.lockedBySomeoneElse = ( iter != operationResults.end() ) ? true : false;

		iter = operationResults.find( "haveRev" );
		long haveRev = -1;
		if( iter != operationResults.end() && iter->second.IsNumber() )
			iter->second.ToLong( &haveRev );

		iter = operationResults.find( "headRev" );
		long headRev = -1;
		if( iter != operationResults.end() && iter->second.IsNumber() )
			iter->second.ToLong( &headRev );

		fileStatus.haveLatestRevision = ( headRev == haveRev ) ? true : false;
	}
	else
	{
		// In the unmapped case, it may be open for add.
		iter = operationResults.find( "action" );
		if( iter != operationResults.end() )
		{
			if( iter->second == "add" )
			{
				fileStatus.unknown = false;
				fileStatus.underSourceControl = true;		
				fileStatus.openFor = FileStatus::OPEN_FOR_ADD;
				fileStatus.openBySomeoneElse = false;
				fileStatus.haveLatestRevision = true;
				fileStatus.lockedByMe = false;
				fileStatus.lockedBySomeoneElse = false;
			}
		}
	}
}

//=================================================================================
AddOperation::AddOperation( void )
{
}

//=================================================================================
/*virtual*/ AddOperation::~AddOperation( void )
{
}

//=================================================================================
/*virtual*/ bool AddOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "add" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void AddOperation::ProcessResults( OperationResults& operationResults )
{
	OperationResults::iterator iter = operationResults.find( "p4_error" );
	if( iter != operationResults.end() )
	{
		//...
	}
}

//=================================================================================
RevertOperation::RevertOperation( void )
{
}

//=================================================================================
/*virtual*/ RevertOperation::~RevertOperation( void )
{
}

//=================================================================================
/*virtual*/ bool RevertOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "revert" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void RevertOperation::ProcessResults( OperationResults& operationResults )
{
	OperationResults::iterator iter = operationResults.find( "p4_error" );
	if( iter != operationResults.end() )
	{
		//...
	}
}

//=================================================================================
OpenForEditOperation::OpenForEditOperation( void )
{
}

//=================================================================================
/*virtual*/ OpenForEditOperation::~OpenForEditOperation( void )
{
}

//=================================================================================
/*virtual*/ bool OpenForEditOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "edit" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void OpenForEditOperation::ProcessResults( OperationResults& operationResults )
{
}

//=================================================================================
SubmitOperation::SubmitOperation( void )
{
}

//=================================================================================
/*virtual*/ SubmitOperation::~SubmitOperation( void )
{
}

//=================================================================================
/*virtual*/ bool SubmitOperation::Perform( void )
{
	if( !InitializeClient( &file ) )
		return false;

	wxArrayString command;
	command.Add( "submit" );
	command.Add( "-d" );
	if( userName.IsEmpty() )
		userName = "(unknown)";
	command.Add( "PerforceClient plugin for CornucopiaEditor submits on behalf of " + userName );
	command.Add( file );
	if( !IssueCommand( command ) )
		return false;

	if( !FinalizeClient() )
		return false;

	return true;
}

//=================================================================================
/*virtual*/ void SubmitOperation::ProcessResults( OperationResults& operationResults )
{
}

//=================================================================================
GetLatestOperation::GetLatestOperation( void )
{
}

//=================================================================================
/*virtual*/ GetLatestOperation::~GetLatestOperation( void )
{
}

//=================================================================================
/*virtual*/ bool GetLatestOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "sync" );
	command.Add( file + "#head" );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void GetLatestOperation::ProcessResults( OperationResults& operationResults )
{
}

//=================================================================================
LockOperation::LockOperation( void )
{
}

//=================================================================================
/*virtual*/ LockOperation::~LockOperation( void )
{
}

//=================================================================================
/*virtual*/ bool LockOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "lock" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void LockOperation::ProcessResults( OperationResults& operationResults )
{
}

//=================================================================================
UnlockOperation::UnlockOperation( void )
{
}

//=================================================================================
/*virtual*/ UnlockOperation::~UnlockOperation( void )
{
}

//=================================================================================
/*virtual*/ bool UnlockOperation::Perform( void )
{
	wxArrayString command;
	command.Add( "unlock" );
	command.Add( file );
	return InitIssueFinal( command, &file );
}

//=================================================================================
/*virtual*/ void UnlockOperation::ProcessResults( OperationResults& operationResults )
{
}

// PerforceOperation.cpp