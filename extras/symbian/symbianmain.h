#if !defined(SYMBIANMAIN_H)
#define SYMBIANMAIN_H

#include <coecntrl.h>
#include <es_sock.h>
#include <f32file.h>
#include <eikappui.h>
#include <e32des16.h>

class CIoUi;
class CPlainText;
class CEikConsoleScreen;

class CConsoleControl : public CCoeControl
	{
	public:
	  RFs fileServer;
	  RSocketServ socketServer;
	  CPlainText* clipboardText;
	  CEikConsoleScreen* iConsole;	// Standard EIKON console control
	  CIoUi* pUI;
  
public:
	static CConsoleControl* NewL(CIoUi* ui, IoState* vm, TRect& rect);
	~CConsoleControl();
	void ConstructL(CIoUi* ui, IoState* vm, TRect& rect);

	// Override CCoeControl 
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void ActivateL();

	void Print(const TDesC& aDes);
	void Flush();
	void SetFont(TFontSpec& font) const;

protected:
	CConsoleControl(); 

private:
	IoState* pVM;
	char keyBuffer[1024];
	int keyIndex;
	char* historyBuffer[100];
	int historyCount;
	int currentHistoryCount;
	};

class CIoUi : public CEikAppUi
{
public:
	CIoUi(IoState* pVM);
    void ConstructL();
	void CreateConsoleL();

	void DynInitMenuPaneL(TInt resourceID, CEikMenuPane* pane);
	void AddControl(CCoeControl* control);
	void RemoveControl(CCoeControl* control);
	void MakeVisible(CCoeControl* control);
	CEikButtonGroupContainer* GetCba();
	~CIoUi();
private: 
	// Override CEikAppUi
    void HandleCommandL(TInt aCommand);
	void OnExitCommand();

private:
	CConsoleControl* iConsoleControl;
	IoState* pVM;
	RPointerArray<CCoeControl> iControlList;
};

// Utility functions
TPtr16 stringToPtr16(char const* s, int len = -1);

#endif
