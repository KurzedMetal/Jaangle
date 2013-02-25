#ifndef _UINTtoStringMapping_h_
#define _UINTtoStringMapping_h_

class UINTtoStringMapping
{
public:
	UINTtoStringMapping()		{}
	~UINTtoStringMapping()		{}
	BOOL Load(LPCTSTR pathName);
	BOOL Save(LPCTSTR pathName);
	LPCTSTR Get(UINT key) const;
	void Set(UINT key, LPCTSTR value);
	void Remove(UINT key);
	void Clear();
	INT Count()	const							{return m_mapping.size();}
	BOOL IsDirty() const						{return m_bDirty;}
private:
	typedef std::map<UINT, std::basic_string<TCHAR> > U2SMap;
	U2SMap m_mapping;
	BOOL m_bDirty;

};

#endif
