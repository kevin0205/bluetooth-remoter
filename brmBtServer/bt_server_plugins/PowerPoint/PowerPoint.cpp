// PowerPoint.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PowerPoint.h"
#include "PPTinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CPowerPointApp

BEGIN_MESSAGE_MAP(CPowerPointApp, CWinApp)
END_MESSAGE_MAP()


// CPowerPointApp construction

CPowerPointApp::CPowerPointApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPowerPointApp object

CPowerPointApp theApp;


// CPowerPointApp initialization

BOOL CPowerPointApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


////////////////////////////////////////////

class PPT : public PluginBase
{
protected:
	bool initialized;
	CApplication app;

	bool initPP()
	{
		try
		{
			if (!FindWindowW(L"PPTFrameClass", NULL))
			{
				// AfxMessageBox(L"PPT not running");
				
				if (initialized) app.ReleaseDispatch();

				initialized = false;
				return false;
			}
			else if (initialized && !app.get_Visible()) // ppt window closed, we should close the handle so that powerpoint can really quit (it would be still running in the background otherwise)
			{
				// AfxMessageBox(L"PPT not running.. at least not in foreground...");
				app.ReleaseDispatch();

				initialized = false;
				return false;
			}
		}
		catch(COleException * e)
		{
			// I've found this to occur when PP is being closed and the do-you-really-wish-to-quit dialog appears
			/*wchar_t tmp[256];
			e->GetErrorMessage(tmp, 256);
			AfxMessageBox(tmp);*/
			return false;
		}

		if (!initialized)
		{
			COleException e;
			if(!app.CreateDispatch(L"Powerpoint.Application", &e)) {
			   /*CString str;
			   str.Format(L"CreateDispatch() failed w/err 0x%08lx", e.m_sc);
			   AfxMessageBox(str, MB_SETFOREGROUND);*/
			   return false;
			}
			initialized = true;
		}

		return true;
	}

	int getCurrSlide(void) // slides are numbered from 1, so 0 means there is no slideshow open
	{
		if (!initPP()) return 0;

		CSlideShowWindows windows = app.get_SlideShowWindows();
		if (!windows.get_Count()) return 0;

		CSlideShowWindow window = windows.Item(1);
		CSlideShowView view = window.get_View();
		if (view.get_State() != 1 && view.get_State() != 2) return 0;  // ppSlideShowRunning && ppSlideShowPaused
		CSlide slide = view.get_Slide();

		return slide.get_SlideIndex();
	}

public:
	PPT()
	{
		initialized = false;

		actions.insert(std::pair<CString, executableAction>(L"PowerPoint: Next", executableAction(false, L"", this, 1)) );
		actions.insert(std::pair<CString, executableAction>(L"PowerPoint: Previous", executableAction(false, L"", this, 2)) );

		infoCollectors.insert(std::pair<CString, executableAction>(L"PowerPoint: List of slides", executableAction(true, L"Number of titles to display (default is 5)", this, 1)) );
		infoCollectors.insert(std::pair<CString, executableAction>(L"PowerPoint: Slide notes", executableAction(false, L"", this, 2)) );
	}

	~PPT()
	{
		if (initialized) app.ReleaseDispatch();
	}

	void next()
	{
		if (!initPP()) return;

		CSlideShowWindows windows = app.get_SlideShowWindows();
		if (!windows.get_Count()) return; // no slideshow open

		CSlideShowWindow window = windows.Item(1);
		CSlideShowView view = window.get_View();
		view.Next();
	}

	void prev()
	{
		if (!initPP()) return;

		CSlideShowWindows windows = app.get_SlideShowWindows();
		if (!windows.get_Count()) return; // no slideshow open

		CSlideShowWindow window = windows.Item(1);
		CSlideShowView view = window.get_View();
		view.Previous();
	}
	
	void callAction(int actionId, CString params, bool keyDown)
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED); // make OLE multrithread-aware

		switch(actionId)
		{
		case 1: if (keyDown) next(); break;
		case 2: if (keyDown) prev(); break;
		}
	}

	CString getNotes ()
	{
		static int lastSlide = 0;
		static CString s(L"");

		int currSlide = getCurrSlide();

		if (currSlide && currSlide == lastSlide) return s;

		lastSlide = currSlide;

		if (!currSlide) return L"";

		CSlideShowWindows windows = app.get_SlideShowWindows();
		if (!windows.get_Count()) return L"";

		CSlideShowWindow window = windows.Item(1);
		CSlideShowView view = window.get_View();
		if (view.get_State() != 1 && view.get_State() != 2) return L"";  // ppSlideShowRunning && ppSlideShowPaused

		CSlide slide = view.get_Slide();
		CSlideRange notespage = slide.get_NotesPage();
		CShapes shapes = notespage.get_Shapes();

		for (int i = 1; i <= shapes.get_Count(); i++)
		{
			VARIANT v;
			v.vt = VT_I4;
			v.lVal = i;
			CShape shape = shapes.Item(v);
			if (shape.get_Type() != 14) continue; // msoPlaceholder
			CPlaceholderFormat pf = shape.get_PlaceholderFormat();
			if (pf.get_Type() != 2) continue; // ppPlaceholderBody

			CTextFrame tf = shape.get_TextFrame();
			CTextRange tr = tf.get_TextRange();

			s = tr.get_Text();
			s.Replace(L"\r\n", L"\n");
			s.Replace(13, 10);
			return s;
		}

		return L"";
	}

	CString getSlideList(int nSlides)
	{
		static int lastSlide = 0;
		static CString tmp(L"");

		if (!initPP()) return L"PowerPoint not running";

		int currSlide;
		if (!(currSlide = getCurrSlide())) return L"Please start a slideshow on the computer"; // no slideshow open

		if (currSlide == lastSlide) return tmp;

		lastSlide = currSlide;
		tmp = L"";

		CPresentation presentation = app.get_ActivePresentation();
		CSlides slides = presentation.get_Slides();

		if (!nSlides) nSlides = 5;
		int startSlide = currSlide - (nSlides / 2) < 1 ? 1 : currSlide - (nSlides / 2);
		int endSlide = startSlide + (nSlides - 1) > slides.get_Count() ? slides.get_Count() : startSlide + (nSlides - 1);
		if (endSlide - startSlide < (nSlides - 1)) startSlide = endSlide - (nSlides - 1) > 1 ? endSlide - (nSlides - 1) : 1;

		for (int i = startSlide; i <= endSlide; i++)
		{
			CString n;
			if (currSlide == i) n.Format(L">> %d. ", i); else n.Format(L"%d. ", i);
			tmp += n;

			VARIANT v;
			v.vt = VT_I4;
			v.lVal = i;

			CSlide slide = slides.Item(v);
			CShapes shapes = slide.get_Shapes();

			if (shapes.get_Count())
			{
				int s = shapes.get_Count();

				for (int j = 1; j <= shapes.get_Count(); j++)
				{
					v.lVal = j;
					CShape shape = shapes.Item(v);
					CTextFrame tf = shape.get_TextFrame();
					if (!tf.get_HasText()) continue;

					CTextRange tr = tf.get_TextRange();

					tmp += tr.get_Text();
					break;
				}
			}
			tmp += L"\n";
		}
		
		return tmp;
	}

	CString callInfoCollector(int infoCollectorId, CString params)
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED); // make OLE multrithread-aware

		switch(infoCollectorId)
		{
		case 1:
			int nSlides;
			swscanf(params.GetBuffer(), L"%d", &nSlides);
			return getSlideList(nSlides);
		case 2:	return getNotes();
		}
		return L"";
	}

	void onProfileClosed()
	{
		if (initialized) app.ReleaseDispatch();
		initialized = false;
	}
};




extern "C" __declspec(dllexport) PluginBase * loadPlugin()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new PPT();
}