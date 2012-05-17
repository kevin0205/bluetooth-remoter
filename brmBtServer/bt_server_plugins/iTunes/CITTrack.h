// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard
#pragma once

#import "C:\\Program Files\\iTunes\\iTunes.exe" no_namespace // you might want to change this to Program Files (x86) if you're running on 64-bit windows
// CITTrack wrapper class

class CITTrack : public COleDispatchDriver
{
public:
	CITTrack(){} // Calls COleDispatchDriver default constructor
	CITTrack(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CITTrack(const CITTrack& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IITTrack methods
public:
	void GetITObjectIDs(long * sourceID, long * playlistID, long * trackID, long * databaseID)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x60020000, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sourceID, playlistID, trackID, databaseID);
	}
	CString get_Name()
	{
		CString result;
		InvokeHelper(0x60020001, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Name(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60020001, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Index()
	{
		long result;
		InvokeHelper(0x60020003, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_sourceID()
	{
		long result;
		InvokeHelper(0x60020004, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_playlistID()
	{
		long result;
		InvokeHelper(0x60020005, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_trackID()
	{
		long result;
		InvokeHelper(0x60020006, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_TrackDatabaseID()
	{
		long result;
		InvokeHelper(0x60020007, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void Delete()
	{
		InvokeHelper(0x60030000, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Play()
	{
		InvokeHelper(0x60030001, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	LPDISPATCH AddArtworkFromFile(LPCTSTR filePath)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030002, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, filePath);
		return result;
	}
	long get_Kind()
	{
		long result;
		InvokeHelper(0x60030003, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	LPDISPATCH get_Playlist()
	{
		LPDISPATCH result;
		InvokeHelper(0x60030004, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	CString get_Album()
	{
		CString result;
		InvokeHelper(0x60030005, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Album(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030005, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Artist()
	{
		CString result;
		InvokeHelper(0x60030007, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Artist(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030007, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BitRate()
	{
		long result;
		InvokeHelper(0x60030009, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_BPM()
	{
		long result;
		InvokeHelper(0x6003000a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BPM(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003000a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Comment()
	{
		CString result;
		InvokeHelper(0x6003000c, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Comment(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003000c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Compilation()
	{
		BOOL result;
		InvokeHelper(0x6003000e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Compilation(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6003000e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Composer()
	{
		CString result;
		InvokeHelper(0x60030010, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Composer(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030010, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	DATE get_DateAdded()
	{
		DATE result;
		InvokeHelper(0x60030012, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
		return result;
	}
	long get_DiscCount()
	{
		long result;
		InvokeHelper(0x60030013, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_DiscCount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030013, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_DiscNumber()
	{
		long result;
		InvokeHelper(0x60030015, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_DiscNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030015, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Duration()
	{
		long result;
		InvokeHelper(0x60030017, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL get_Enabled()
	{
		BOOL result;
		InvokeHelper(0x60030018, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Enabled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x60030018, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_EQ()
	{
		CString result;
		InvokeHelper(0x6003001a, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_EQ(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003001a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void put_Finish(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003001c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Finish()
	{
		long result;
		InvokeHelper(0x6003001c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_Genre()
	{
		CString result;
		InvokeHelper(0x6003001e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Genre(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6003001e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Grouping()
	{
		CString result;
		InvokeHelper(0x60030020, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Grouping(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x60030020, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_KindAsString()
	{
		CString result;
		InvokeHelper(0x60030022, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	DATE get_ModificationDate()
	{
		DATE result;
		InvokeHelper(0x60030023, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
		return result;
	}
	long get_PlayedCount()
	{
		long result;
		InvokeHelper(0x60030024, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_PlayedCount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030024, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	DATE get_PlayedDate()
	{
		DATE result;
		InvokeHelper(0x60030026, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
		return result;
	}
	void put_PlayedDate(DATE newValue)
	{
		static BYTE parms[] = VTS_DATE ;
		InvokeHelper(0x60030026, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_PlayOrderIndex()
	{
		long result;
		InvokeHelper(0x60030028, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_Rating()
	{
		long result;
		InvokeHelper(0x60030029, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Rating(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030029, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_SampleRate()
	{
		long result;
		InvokeHelper(0x6003002b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_Size()
	{
		long result;
		InvokeHelper(0x6003002c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_Start()
	{
		long result;
		InvokeHelper(0x6003002d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Start(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6003002d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Time()
	{
		CString result;
		InvokeHelper(0x6003002f, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long get_TrackCount()
	{
		long result;
		InvokeHelper(0x60030030, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_TrackCount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030030, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_TrackNumber()
	{
		long result;
		InvokeHelper(0x60030032, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_TrackNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030032, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_VolumeAdjustment()
	{
		long result;
		InvokeHelper(0x60030034, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_VolumeAdjustment(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030034, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Year()
	{
		long result;
		InvokeHelper(0x60030036, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Year(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x60030036, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Artwork()
	{
		LPDISPATCH result;
		InvokeHelper(0x60030038, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}

	// IITTrack properties
public:

};
