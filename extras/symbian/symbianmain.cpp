#include <e32base.h>
#include <eikappui.h>
#include <eikapp.h>
#include <e32cons.h>
#include <eikconso.h>
#include <eikdoc.h>
#include <baclipb.h>
#include <s32std.h>
#include <string.h>
#include <txtetext.h>
#include <txtrich.h>
#include <charconv.h>
#include <coecobs.h>
#include <eikbtgpc.h>
#include <eikenv.h>
#include <io.rsg>

extern "C"
{
#include "IoState.h"
#include "IoNIL.h"
#include "IoMessage.h"
#include "IoNumber.h"
}

#include "SymbianSockets.h"
#include "SymbianMain.h"

class CConsoleControl;

void executeClipboard(IoState* state);

extern "C"
{
	void* init_thread_globals();

	struct thread_globals* get_thread_globals()
	{
		return (struct thread_globals*)Dll::Tls();
	}

	void Scheduler_current_(Scheduler* s)
	{
		Dll::SetTls(s);
	}

	Scheduler *Scheduler_current(void) 
	{ 
		return (Scheduler*)Dll::Tls(); 
	}


	IoMessage *IoMessage_newFromText_label_(void *state, char *text, char *label);
	int IoObject_activeCoroutineCount(void) ;
	void IoObject_yield(IoObject *self);

	void IoExceptionCatch_jump(IoExceptionCatch *self)
	{ 
		User::Leave(1);
	}

	IoValue *IoObject_catchException(IoObject *self, IoObject *locals, IoMessage *m)
	{
	  IoValue *result;
	  IoMessage_assertArgCount_(m, 3);
	  {
		 IoString *exceptionName = (IoString *)IoMessage_locals_stringArgAt_(m, locals, 0);
		 IoExceptionCatch *eCatch = IoState_pushExceptionCatchWithName_((IoState*)self->tag->state, 
		   CSTRING(exceptionName));

		 TInt r = 0;
		 TRAP(r, result = (IoValue*)IoMessage_locals_valueArgAt_(m, locals, 1);
				 IoState_popExceptionCatch_((IoState*)self->tag->state, eCatch););

		if(r != 0)
		{ 
		  IoObject_setSlot_to_((IoObject*)locals, USTRING("exceptionName"), eCatch->caughtName);
		  if (eCatch->caughtDescription)
		  { 
			IoObject_setSlot_to_(locals, USTRING("exceptionDescription"), 
			  eCatch->caughtDescription); 
		  }
		  else
		  { 
			IoObject_setSlot_to_(locals, USTRING("exceptionDescription"), 
			  USTRING("<no description>")); 
		  }      
		  IoState_popExceptionCatch_((IoState*)self->tag->state, eCatch);
		  result = (IoValue*)IoMessage_locals_valueArgAt_(m, locals, 2);
		}
	  }
	  if (!result) { return IONIL(self); }
	  return result;
	}

	IoValue *IoState_doCString_(IoState *self, char *s, int debug, char *label)
	{
	  IoValue * volatile result = self->ioNil;
	  IoExceptionCatch * volatile eCatch = IoState_pushExceptionCatchWithName_(self, "");
	  TInt r = 0;
	  TRAP(r,
	  {
		IoMessage *m = IoMessage_newFromText_label_(self, s, label);
	    IoState_stackRetain_(self, (IoValue *)m);
		if (m)
		{
		  /*
		  IoBlock *block = IoBlock_new(self);
		  IoBlock_message_(block, m);
		  */
		  if (debug)
		  { IoState_print_(self, "parsed: "); IoMessage_print(m); IoState_print_(self, "\n"); }
		  result = (IoValue*)IoMessage_locals_performOn_(m, self->lobby, self->lobby);
		  /*result = IoBlock_target_locals_call_(block, self->lobby, self->lobby, m);*/
		  while (Scheduler_coroCount(self->scheduler) > 1) 
		  { IoObject_yield(self->mainActor); }
		}
	  });

	  if(r != 0)
	  { IoState_callErrorCallback(self, eCatch->caughtName, eCatch->caughtDescription); }
	  IoState_popExceptionCatch_(self, (IoExceptionCatch *)eCatch);
	  return (IoValue *)result;
	}

	void ProcessUIEvent()
	{
		TRequestStatus status;
		RTimer timer;
		timer.CreateLocal();
		timer.After(status, 10000);
		User::WaitForAnyRequest();
		if(status == KRequestPending)
		{
			TInt error;
			CActiveScheduler::RunIfReady(error, CActive::EPriorityIdle);
			User::WaitForRequest(status);
		}
		timer.Close();
	}
}

CConsoleControl::CConsoleControl() : 
	 keyIndex(0), 
	 clipboardText(0),
	 historyCount(0),
	 currentHistoryCount(0)
{
	socketServer.Connect();
	fileServer.Connect();
	memset(historyBuffer, 0, sizeof(historyBuffer));
}

void CConsoleControl::SetFont(TFontSpec& font) const 
{ 
	iConsole->SetFontL(font); 
}

void CIoUi::AddControl(CCoeControl* control)
{
	iControlList.Append(control);
}

void CIoUi::RemoveControl(CCoeControl* control)
{
	TInt f = iControlList.Find(control);
	if(f != KErrNotFound)
	{
		iControlList.Remove(f);
	}
}

CEikButtonGroupContainer* CIoUi::GetCba()
{
	return CEikButtonGroupContainer::Current();
}

TUint16* stringToUint16(char const* s, int len = -1)
{
	if(len == -1)
	{
		len = strlen(s);
	}
	TUint16* buffer = new TUint16[len];
	for(int index = 0; index < len; ++index)
	{
		buffer[index] = s[index];
	}

	return buffer;
}

TPtr16 stringToPtr16(char const* s, int len)
{
	if(len == -1)
	{
		len = strlen(s);
	}
	return TPtr16(stringToUint16(s, len), len, len);
}

void MyPrint(void* state, char* s)
{
	IoState* pVM = (IoState*)state;
	if(IoState_userData(pVM))
	{
		CConsoleControl* control = reinterpret_cast<CConsoleControl*>(IoState_userData(pVM));
		TPtrC ptr(stringToUint16(s), strlen(s));
		control->Print(ptr);
		if(control->clipboardText)
		{
			control->clipboardText->InsertL(control->clipboardText->DocumentLength(), ptr);
		}
		control->Flush();
	}
}

void MyError(void* pVM, char* s, char* s2)
{
	MyPrint(pVM, s);
}

void MyExit(void* pVM)
{
}

CConsoleControl* CConsoleControl::NewL(CIoUi* ui, IoState* vm, TRect& rect)
	{
	CConsoleControl* self=new (ELeave) CConsoleControl;
	CleanupStack::PushL(self);
	self->ConstructL(ui, vm, rect);
	self->SetFocus(true);
	CleanupStack::Pop();
	return self;
	}

void CConsoleControl::ConstructL(CIoUi* ui, IoState* vm, TRect& rect)
	{
	pUI = ui;
	pVM = vm;
	IoState_userData_(pVM, this);
    CreateWindowL();
	SetRect(rect);
    EnableDragEvents();
	SetBlank();

    iConsole=new(ELeave) CEikConsoleScreen;
	_LIT(KFicl,"Io v20020910");
	TPoint p1 = Position();
	TPoint p2 = PositionRelativeToScreen();
	TSize s = Size();
	iConsole->ConstructL(KFicl, Position(), s, 0, EEikConsWinInPixels); 
	iConsole->SetHistorySizeL(100,100);
	iConsole->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn);

	TPtrC fptr(stringToUint16("Swiss"), strlen("Swiss"));
	TFontSpec font(fptr, 120);
	SetFont(font);

	}

CConsoleControl::~CConsoleControl()
	{
	fileServer.Close();
	IoState_userData_(pVM, 0);
	delete iConsole;
	pVM = 0;
	}

void CConsoleControl::ActivateL()
	{
	CCoeControl::ActivateL();
	iConsole->SetKeepCursorInSight(TRUE);
	iConsole->DrawCursor();
	iConsole->ConsoleControl()->SetFocus(ETrue, EDrawNow); 
	}

TKeyResponse CConsoleControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
	if (aType!=EEventKey)
		return(EKeyWasConsumed);
    TInt aChar=aKeyEvent.iCode;

	if(aChar == EKeyEnter)
	{
		Print(TPtrC(stringToUint16("\n"), strlen("\n")));
		Flush();
		keyBuffer[keyIndex] = 0;
		if(strcmp(keyBuffer, "/ec") == 0)
		{
			clipboardText = CPlainText::NewL();
			TInt r = 0;
			TRAP(r, executeClipboard(pVM));
			if(r)
			{
				char buffer[200];
				sprintf(buffer, "Failed: Leave code %d\n", r);
				MyPrint(pVM, buffer);
			}
			CClipboard* cb = CClipboard::NewForWritingLC(fileServer);
			clipboardText->CopyToStoreL(cb->Store(), cb->StreamDictionary(), 0, clipboardText->DocumentLength());
			delete clipboardText;
			clipboardText = 0;
			cb->CommitL();
			CleanupStack::PopAndDestroy();
		}
		else
		{
			TInt r = 0;
			IoState_cliInput(pVM, keyBuffer);
		}


		if(historyCount == 100)
		{
			delete[] historyBuffer[0];

			for(int index = 1; index < 100; ++index)
			{
				historyBuffer[index - 1] = historyBuffer[index];
			}
			historyCount--;
		}

		historyBuffer[historyCount] = new char[strlen(keyBuffer) + 1];
		strcpy(historyBuffer[historyCount], keyBuffer);
		historyCount++;
		currentHistoryCount = historyCount;

		memset(keyBuffer, 0, sizeof(keyBuffer));
		keyBuffer[0] = 0;
		keyIndex = 0;
	}
	else if(aChar == EKeyBackspace)
	{
		if(keyIndex > 0)
		{
			iConsole->Left(1);
			Print(TPtrC(stringToUint16(" "), strlen(" ")));
			iConsole->Left(1);
			--keyIndex; 
		}
	}
	else if(aChar == EKeyLeftArrow)
	{
		if(keyIndex > 0)
		{
			iConsole->Left(1);
			--keyIndex; 
		}
	}
	else if(aChar == EKeyRightArrow)
	{		
		iConsole->Right(1);
		if(keyBuffer[keyIndex] == 0)
		{
			keyBuffer[keyIndex] = ' ';
		}
		keyIndex++;
	}
	else if(aChar == EKeyUpArrow)
	{
		if(currentHistoryCount > 0)
		{
			strcpy(keyBuffer, historyBuffer[--currentHistoryCount]);
			TPoint pos = iConsole->CursorPos();
			if(keyIndex > 0)
			{
				iConsole->SetCursorPosAbs(TPoint(pos.iX - keyIndex , pos.iY));
				iConsole->ClearToEndOfLine();
			} 

			iConsole->SetCursorPosAbs(TPoint(pos.iX - keyIndex , pos.iY));
			Print(TPtrC(stringToUint16(keyBuffer)));
			Flush();
			keyIndex = strlen(keyBuffer) + 1;
		}
	}
	else
	{
		char buffer[2];
		buffer[0] = char(aChar);
		buffer[1] = 0;
		Print(TPtrC(stringToUint16(buffer), strlen(buffer)));
		Flush();
		keyBuffer[keyIndex++] = char(aChar);
	}
	
    return(EKeyWasConsumed);
    }

void CConsoleControl::Print(const TDesC& aDes)
{
	iConsole->Write(aDes);
}

void CConsoleControl::Flush()
{
	iConsole->FlushChars();
}


CIoUi::CIoUi(IoState* vm) : pVM(vm), iConsoleControl(0)
	{
	}

void CIoUi::ConstructL()
    {
    BaseConstructL();
	CreateConsoleL();
	_LIT(KCommands,"\nio v20020925 Started\n\n");

	iConsoleControl->Print(KCommands);
    }

void CIoUi::CreateConsoleL()
	{	
	TRect rect(ClientRect());
	iConsoleControl=CConsoleControl::NewL(this, pVM, rect);
	IoState_userData_(pVM, iConsoleControl);
	AddToStackL(iConsoleControl);
	iConsoleControl->ActivateL();
	}

CIoUi::~CIoUi()
	{
    delete(iConsoleControl);
	}

void CIoUi::HandleCommandL(TInt aCommand)
	{
		switch (aCommand)
			{
		case EEikCmdExit:
			OnExitCommand();
			break;
		default:;
		}
	}

void CIoUi::DynInitMenuPaneL(TInt resourceID, CEikMenuPane* pane)
{
	if(resourceID == R_CONS_EXTEND_MENU)
	{
		// What to do here
	}
}

void CIoUi::MakeVisible(CCoeControl* control)
{
	if(iConsoleControl->IsVisible())
	{
		if(iControlList.Count() > 0)
		{
			iConsoleControl->MakeVisible(false);
			RemoveFromStack(iConsoleControl);
			control->MakeVisible(true);
			AddToStackL(control);
		}
	}
	else
	{
		int count = iControlList.Count();
		for(int i = 0; i < count; ++i)
		{
			if(iControlList[i]->IsVisible())
			{
				iControlList[i]->MakeVisible(false);
				RemoveFromStack(iControlList[i]);
			}
		}
		control->MakeVisible(true);
		AddToStackL(control);
	}
}

void CIoUi::OnExitCommand()
{
	Exit();
}

class CIoDocument : public CEikDocument
	{
public:
	CIoDocument(CEikApplication& aApp);
	static CIoDocument* NewL(CEikApplication& aApp);
	~CIoDocument();
	void ConstructL();
private: 
	// Override CApaDocument
	CEikAppUi* CreateAppUiL();
private: 
	IoState* pState;
	};

CIoDocument::CIoDocument(CEikApplication& aApp)
	: CEikDocument(aApp), pState(0) 
{ 
}

CIoDocument* CIoDocument::NewL(CEikApplication& aApp)
	{
	CIoDocument* self=new (ELeave) CIoDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CIoDocument::ConstructL()
	{		
		pState = IoState_new();
		IoState_userData_(pState, 0);
		IoState_printCallback_(pState, MyPrint);
		IoState_errorCallback_(pState, MyError);
		IoState_exitCallback_(pState, MyExit);
		IoState_pauseGarbageCollector(pState);
		initSocketAddons(pState);
	    IoState_resumeGarbageCollector(pState);

	}

CIoDocument::~CIoDocument()
	{
	    IoState_free(pState);
	}

CEikAppUi* CIoDocument::CreateAppUiL()
	{
    return(new(ELeave) CIoUi(pState));
	}

class CIoApplication : public CEikApplication
	{
private: // from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

TUid CIoApplication::AppDllUid() const
	{
#ifdef _UNICODE
const TUid KUidFiclApp = {0x10004854};
#else
const TUid KUidFiclApp = {0x10004850};
#endif

	return(KUidFiclApp);
	}

CApaDocument* CIoApplication::CreateDocumentL()
	{
	return CIoDocument::NewL(*this);
	}

EXPORT_C CApaApplication* NewApplication()
	{
	return(new CIoApplication);
	}

GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}

TPtr8* narrowTptr16(RFs& fs, TPtr16* p16, int delete16) 
{
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewL();
	converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, fs);


	TPtrC16 originalText(*p16);
	TInt length = originalText.Length() * 3;
	TUint8* finalBuffer = new TUint8[length + 1];
	memset(finalBuffer, 0, length + 1);
	TInt currentIndex = 0;

	TBuf8<100> tempBuffer;

	TInt result = converter->ConvertFromUnicode(tempBuffer, originalText);
	if(tempBuffer.Length() >= length)
	{
		TUint8* newBuffer = new TUint8[length * 2];
		memset(newBuffer, 0, length * 2);
		memcpy(newBuffer, finalBuffer, length);
		length = length * 2;
		delete[] finalBuffer;
		finalBuffer = newBuffer;
	}

	strncpy((char*)finalBuffer + currentIndex, (char*)tempBuffer.Ptr(), tempBuffer.Length());
	currentIndex += tempBuffer.Length();

	while(result > 0)
	{
		originalText.Set(originalText.Right(result));
		result = converter->ConvertFromUnicode(tempBuffer, originalText);
		if(currentIndex + tempBuffer.Length() >= length)
		{
			TUint8* newBuffer = new TUint8[length * 2];
			memset(newBuffer, 0, length * 2);
			memcpy(newBuffer, finalBuffer, length);
			length = length * 2;
			delete[] finalBuffer;
			finalBuffer = newBuffer;
		}

		strncpy((char*)finalBuffer + currentIndex, (char*)tempBuffer.Ptr(), tempBuffer.Length());
		currentIndex += tempBuffer.Length();
	} 

	if(delete16 != 0)
	{
		delete p16;
	}
		
	delete converter;

	return new TPtr8(finalBuffer, currentIndex, length);
}

void executeClipboard(IoState* pVM)
{
	RFs& fs = ((CConsoleControl*)IoState_userData(pVM))->fileServer;
	CClipboard* cb = CClipboard::NewForReadingL(fs);
	TStreamId id = (cb->StreamDictionary()).At(KClipboardUidTypePlainText);
	if(id == KNullStreamId)
	{
		MyPrint(pVM, "Clipboard execution failed\n");
	}

	CPlainText* ptext = CPlainText::NewL();
	ptext->PasteFromStoreL(cb->Store(), cb->StreamDictionary(), 0);
	TUint16 *buffer = new TUint16[ptext->DocumentLength()]; 
	TPtr16 *text = new TPtr16(buffer, ptext->DocumentLength());
	ptext->Extract(*text);

	TPtr8* ptr8 = narrowTptr16(fs, text, 1);
	delete[] buffer;

	char* ctext = new char[ptr8->Length() + 1];
	for(int index = 0; index < ptr8->Length(); ++index)
	{
		if((*ptr8)[index] == '\t' ||
			(*ptr8)[index] == 26)
			ctext[index] = ' ';
		else
			ctext[index] = (*ptr8)[index];
	}
	ctext[ptr8->Length()] = 0;
	IoState_cliInput(pVM, ctext);

	delete cb;
	delete[] ctext;
	delete ptr8;
}

